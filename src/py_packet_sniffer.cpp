/**
 * @file py_packet_sniffer.cpp
 * @brief Unified packet sniffer for both StoC and CToS traffic.
 */

#include "py_packet_sniffer.h"
#include "Py4GW.h"

#include <algorithm>
#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace {
    constexpr const char* kPacketSnifferModule = "PacketSniffer";

    void LogScanError(const std::string& message) {
        Logger::Instance().LogError(message, kPacketSnifferModule);
    }

    void LogScanInfo(const std::string& message) {
        Logger::Instance().LogInfo("[" + std::string(kPacketSnifferModule) + "] " + message);
    }

    using SendPacketFn = void(__cdecl*)(void* ctx, uint32_t size, const void* packet);
    SendPacketFn CToSTrampoline = nullptr;

    using namespace GW;

    typedef bool(__cdecl* StoCHandler_pt)(Packet::StoC::PacketBase* pak);
    struct StoCHandlerLocal {
        uint32_t* packet_template;
        uint32_t template_size;
        StoCHandler_pt handler_func;
    };
    typedef Array<StoCHandlerLocal> StoCHandlerArrayLocal;

    struct GameServerLocal {
        uint8_t h0000[8];
        struct {
            uint8_t h0000[12];
            struct {
                uint8_t h0000[12];
                void* next;
                uint8_t h0010[12];
                uint32_t ClientCodecArray[4];
                StoCHandlerArrayLocal handlers;
            }* ls_codec;
            uint8_t h0010[12];
            uint32_t ClientCodecArray[4];
            StoCHandlerArrayLocal handlers;
        }* gs_codec;
    };

    constexpr const char* kCToSWrapperPattern = "\xF7\xD8\xC7\x47\x54\x01\x00\x00\x00\x1B\xC0\x25";
    constexpr const char* kCToSWrapperMask = "xxxxxxxxxxxx";

    bool SafeReadCToSHeader(const void* src, uint32_t& out_header) {
        if (!src) {
            return false;
        }

        __try {
            const uint32_t raw_header = *reinterpret_cast<const uint32_t*>(src);
            out_header = static_cast<uint16_t>(raw_header & 0xFFFF);
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            out_header = 0;
            return false;
        }
    }

    uint32_t SafeCopyCToSPacket(const void* src, uint32_t packet_size, std::vector<uint8_t>& out_data) {
        if (!src || packet_size == 0 || packet_size > PacketSniffer::kMaxReasonableCToSPacketSize) {
            out_data.clear();
            return 0;
        }

        __try {
            out_data.resize(packet_size);
            memcpy(out_data.data(), src, packet_size);
            return packet_size;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            out_data.clear();
            return 0;
        }
    }

    uint32_t SafeCopyStoCPacket(const void* src, uint32_t copy_size, std::vector<uint8_t>& out_data) {
        if (!src || copy_size == 0) {
            out_data.clear();
            return 0;
        }

        __try {
            out_data.resize(copy_size);
            memcpy(out_data.data(), src, copy_size);
            return copy_size;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            out_data.resize(sizeof(GW::Packet::StoC::PacketBase));
            memcpy(out_data.data(), src, sizeof(GW::Packet::StoC::PacketBase));
            return sizeof(GW::Packet::StoC::PacketBase);
        }
    }

    enum class StoCFieldType {
        Ignore,
        AgentId,
        Float,
        Vect2,
        Vect3,
        Byte,
        Word,
        Dword,
        Blob,
        String16,
        Array8,
        Array16,
        Array32,
        NestedStruct,
        Unknown,
    };

    StoCFieldType GetStoCFieldType(uint32_t type, uint32_t size, uint32_t count) {
        switch (type) {
        case 0: return StoCFieldType::AgentId;
        case 1: return StoCFieldType::Float;
        case 2: return StoCFieldType::Vect2;
        case 3: return StoCFieldType::Vect3;
        case 4:
        case 8:
            switch (count) {
            case 1: return StoCFieldType::Byte;
            case 2: return StoCFieldType::Word;
            case 4: return StoCFieldType::Dword;
            default: break;
            }
            break;
        case 5:
        case 9:
            return StoCFieldType::Blob;
        case 6:
        case 10:
            return StoCFieldType::Ignore;
        case 7:
            return StoCFieldType::String16;
        case 11:
            switch (size) {
            case 1: return StoCFieldType::Array8;
            case 2: return StoCFieldType::Array16;
            case 4: return StoCFieldType::Array32;
            default: break;
            }
            break;
        case 12:
            return StoCFieldType::NestedStruct;
        default:
            break;
        }
        return StoCFieldType::Unknown;
    }

    bool AdvanceStoCCursor(const uint8_t*& cursor, const uint8_t* max_end, size_t bytes) {
        if (cursor > max_end || bytes > static_cast<size_t>(max_end - cursor)) {
            return false;
        }
        cursor += bytes;
        return true;
    }

    bool MeasureStoCNestedFields(const uint32_t* fields, uint32_t field_count, uint32_t repeat,
        const uint8_t*& cursor, const uint8_t* max_end) {
        if (!fields || repeat > 1024) {
            return false;
        }

        for (uint32_t rep = 0; rep < repeat; ++rep) {
            for (uint32_t i = 0; i < field_count; ++i) {
                const uint32_t field = fields[i];
                const uint32_t type = (field >> 0) & 0xF;
                const uint32_t size = (field >> 4) & 0xF;
                const uint32_t count = (field >> 8) & 0xFFFF;

                switch (GetStoCFieldType(type, size, count)) {
                case StoCFieldType::Ignore:
                    break;
                case StoCFieldType::AgentId:
                case StoCFieldType::Float:
                case StoCFieldType::Byte:
                case StoCFieldType::Word:
                case StoCFieldType::Dword:
                    if (!AdvanceStoCCursor(cursor, max_end, sizeof(uint32_t))) return false;
                    break;
                case StoCFieldType::Vect2:
                    if (!AdvanceStoCCursor(cursor, max_end, sizeof(float) * 2)) return false;
                    break;
                case StoCFieldType::Vect3:
                    if (!AdvanceStoCCursor(cursor, max_end, sizeof(float) * 3)) return false;
                    break;
                case StoCFieldType::Blob:
                    if (!AdvanceStoCCursor(cursor, max_end, count)) return false;
                    break;
                case StoCFieldType::String16:
                    if (!AdvanceStoCCursor(cursor, max_end, static_cast<size_t>(count) * sizeof(wchar_t))) return false;
                    break;
                case StoCFieldType::Array8:
                    if (!AdvanceStoCCursor(cursor, max_end, count)) return false;
                    break;
                case StoCFieldType::Array16:
                    if (!AdvanceStoCCursor(cursor, max_end, sizeof(uint32_t) + (static_cast<size_t>(count) * sizeof(uint16_t)))) return false;
                    break;
                case StoCFieldType::Array32:
                    if (!AdvanceStoCCursor(cursor, max_end, static_cast<size_t>(count) * sizeof(uint32_t))) return false;
                    break;
                case StoCFieldType::NestedStruct: {
                    if (cursor > max_end || sizeof(uint32_t) > static_cast<size_t>(max_end - cursor)) {
                        return false;
                    }
                    uint32_t struct_count = 0;
                    memcpy(&struct_count, cursor, sizeof(struct_count));
                    cursor += sizeof(uint32_t);
                    if (struct_count > 1024) {
                        return false;
                    }
                    if (!MeasureStoCNestedFields(fields + i + 1, field_count - i - 1, struct_count, cursor, max_end)) {
                        return false;
                    }
                    return true;
                }
                case StoCFieldType::Unknown:
                    return false;
                }
            }
        }
        return true;
    }

    uint32_t MeasureStoCPacketSize(const uint32_t* fields, uint32_t field_count, const GW::Packet::StoC::PacketBase* packet) {
        if (!fields || field_count == 0 || !packet) {
            return sizeof(GW::Packet::StoC::PacketBase);
        }

        __try {
            const auto* start = reinterpret_cast<const uint8_t*>(packet);
            const uint8_t* cursor = start + sizeof(uint32_t);
            const uint8_t* max_end = start + PacketSniffer::kMaxStoCPacketBuffer;
            if (!MeasureStoCNestedFields(fields + 1, field_count - 1, 1, cursor, max_end)) {
                return sizeof(GW::Packet::StoC::PacketBase);
            }

            const size_t measured_size = static_cast<size_t>(cursor - start);
            if (measured_size < sizeof(GW::Packet::StoC::PacketBase) || measured_size > PacketSniffer::kMaxStoCPacketBuffer) {
                return sizeof(GW::Packet::StoC::PacketBase);
            }
            return static_cast<uint32_t>(measured_size);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return sizeof(GW::Packet::StoC::PacketBase);
        }
    }

    StoCHandlerArrayLocal* ResolveStoCHandlerArray() {
        uintptr_t address = Scanner::Find("\x75\x04\x33\xC0\x5D\xC3\x8B\x41\x08\xA8\x01\x75", "xxxxxxxxxxxx", -6);
        if (!address) {
            LogScanError("Failed to resolve StoC handler array pattern scan.");
            return nullptr;
        }

        uintptr_t stoc_handler_addr = *(uintptr_t*)address;
        if (!stoc_handler_addr) {
            std::ostringstream oss;
            oss << "StoC handler array pointer read from scan result was null. scan_result=0x" << std::hex << address;
            LogScanError(oss.str());
            return nullptr;
        }

        GameServerLocal** addr = reinterpret_cast<GameServerLocal**>(stoc_handler_addr);
        if (!addr || !*addr || !(*addr)->gs_codec) {
            std::ostringstream oss;
            oss << "StoC handler array context was invalid after scan. scan_result=0x" << std::hex << address
                << " stoc_handler_addr=0x" << stoc_handler_addr
                << " addr=" << addr;
            LogScanError(oss.str());
            return nullptr;
        }

        std::ostringstream oss;
        oss << "Resolved StoC handler array. scan_result=0x" << std::hex << address
            << " stoc_handler_addr=0x" << stoc_handler_addr;
        LogScanInfo(oss.str());
        return &(*addr)->gs_codec->handlers;
    }

    uintptr_t FindCToSTarget() {
        uintptr_t addr = GW::Scanner::Find(kCToSWrapperPattern, kCToSWrapperMask, -0xBF);
        if (addr) {
            std::ostringstream oss;
            oss << "Resolved CToS wrapper via primary pattern. address=0x" << std::hex << addr;
            LogScanInfo(oss.str());
            return addr;
        }
        LogScanError("Primary CToS wrapper pattern scan failed.");

        addr = GW::Scanner::FindAssertion("MsgConn.cpp", "bytes >= sizeof(dword)", 0, 0);
        if (addr) {
            addr = GW::Scanner::ToFunctionStart(addr);
            if (addr) {
                std::ostringstream oss;
                oss << "Resolved CToS wrapper via assertion fallback 'bytes >= sizeof(dword)'. address=0x" << std::hex << addr;
                LogScanInfo(oss.str());
                return addr;
            }
            LogScanError("CToS assertion fallback 'bytes >= sizeof(dword)' matched but ToFunctionStart returned 0.");
        }
        else {
            LogScanError("CToS assertion fallback 'bytes >= sizeof(dword)' failed.");
        }

        addr = GW::Scanner::FindAssertion("MsgConn.cpp", "bytes && data", 0, 0);
        if (addr) {
            addr = GW::Scanner::ToFunctionStart(addr);
            if (addr) {
                std::ostringstream oss;
                oss << "Resolved CToS wrapper via assertion fallback 'bytes && data'. address=0x" << std::hex << addr;
                LogScanInfo(oss.str());
                return addr;
            }
            LogScanError("CToS assertion fallback 'bytes && data' matched but ToFunctionStart returned 0.");
        }
        else {
            LogScanError("CToS assertion fallback 'bytes && data' failed.");
        }

        LogScanError("Failed to resolve any CToS packet send target.");

        return 0;
    }

    static void __cdecl Detour_SendPacket(void* ctx, uint32_t size, const void* packet) {
        if (packet && size >= sizeof(uint32_t) && size <= PacketSniffer::kMaxReasonableCToSPacketSize) {
            PacketLogEntry entry{};
            entry.tick = Py4GW::Get_Tick_Count64();
            entry.direction = PacketDirection::CToS;

            if (SafeReadCToSHeader(packet, entry.header)) {
                entry.size = SafeCopyCToSPacket(packet, size, entry.data);
                if (entry.size >= sizeof(uint32_t) && !entry.data.empty()) {
                    PacketSniffer::Instance().LogPacket(std::move(entry));
                }
            }
        }

        if (CToSTrampoline) {
            CToSTrampoline(ctx, size, packet);
        }
    }
}

PacketSniffer& PacketSniffer::Instance() {
    static PacketSniffer instance;
    return instance;
}

bool PacketSniffer::Initialize() {
    const bool stoc_ok = InitializeStoC();
    const bool ctos_ok = InitializeCToS();
    if (!stoc_ok || !ctos_ok) {
        std::ostringstream oss;
        oss << "Unified packet sniffer initialization failed. StoC=" << stoc_ok << " CToS=" << ctos_ok;
        LogScanError(oss.str());
    }
    return stoc_ok && ctos_ok;
}

bool PacketSniffer::InitializeStoC() {
    if (stoc_initialized_) {
        return true;
    }

    stoc_packet_fields_.assign(STOC_HEADER_COUNT, nullptr);
    stoc_packet_field_counts_.assign(STOC_HEADER_COUNT, 0);
    auto* handlers = ResolveStoCHandlerArray();
    if (!handlers) {
        LogScanError("StoC initialization aborted because the handler array could not be resolved.");
        return false;
    }

    const auto count = std::min<size_t>(handlers->size(), stoc_packet_fields_.size());
    for (size_t i = 0; i < count; ++i) {
        const auto& handler = handlers->at(static_cast<uint32_t>(i));
        stoc_packet_fields_[i] = handler.packet_template;
        stoc_packet_field_counts_[i] = handler.template_size;
    }

    for (uint32_t header = 0; header < STOC_HEADER_COUNT; ++header) {
        GW::StoC::RegisterPacketCallback(
            &stoc_hook_entries_[header],
            header,
            [this](GW::HookStatus*, GW::Packet::StoC::PacketBase* pak) {
                PacketLogEntry entry{};
                entry.tick = Py4GW::Get_Tick_Count64();
                entry.direction = PacketDirection::StoC;
                entry.header = pak->header;

                const uint32_t packet_size = pak->header < stoc_packet_fields_.size()
                    ? MeasureStoCPacketSize(stoc_packet_fields_[pak->header], stoc_packet_field_counts_[pak->header], pak)
                    : sizeof(GW::Packet::StoC::PacketBase);
                const uint32_t copy_size = std::min<uint32_t>(packet_size, static_cast<uint32_t>(kMaxStoCPacketBuffer));
                const uint32_t copied = SafeCopyStoCPacket(pak, copy_size, entry.data);
                entry.size = copied;

                LogPacket(std::move(entry));
            },
            -0x8000);
    }

    stoc_initialized_ = true;
    LogScanInfo("StoC packet sniffer initialized successfully.");
    return true;
}

bool PacketSniffer::InitializeCToS() {
    if (ctos_initialized_) {
        return true;
    }

    const uintptr_t target = FindCToSTarget();
    if (!target) {
        LogScanError("CToS initialization aborted because no packet send target was resolved.");
        return false;
    }

    void* trampoline = nullptr;
    const int result = GW::HookBase::CreateHookRaw(
        reinterpret_cast<void*>(target),
        reinterpret_cast<void*>(Detour_SendPacket),
        &trampoline);

    if (result != 0 || !trampoline) {
        std::ostringstream oss;
        oss << "Failed to create CToS hook. target=0x" << std::hex << target
            << " result=" << std::dec << result
            << " trampoline=" << trampoline;
        LogScanError(oss.str());
        return false;
    }

    CToSTrampoline = reinterpret_cast<SendPacketFn>(trampoline);
    GW::HookBase::EnableHooks(reinterpret_cast<void*>(target));
    ctos_initialized_ = true;
    {
        std::ostringstream oss;
        oss << "CToS packet sniffer initialized successfully. target=0x" << std::hex << target
            << " trampoline=" << trampoline;
        LogScanInfo(oss.str());
    }
    return true;
}

void PacketSniffer::Terminate() {
    TerminateCToS();
    TerminateStoC();
}

void PacketSniffer::TerminateStoC() {
    if (!stoc_initialized_) {
        return;
    }

    for (uint32_t header = 0; header < STOC_HEADER_COUNT; ++header) {
        GW::StoC::RemoveCallback(header, &stoc_hook_entries_[header]);
    }

    stoc_initialized_ = false;
}

void PacketSniffer::TerminateCToS() {
    if (!ctos_initialized_) {
        return;
    }

    const uintptr_t target = FindCToSTarget();
    if (target) {
        GW::HookBase::RemoveHook(reinterpret_cast<void*>(target));
    }

    CToSTrampoline = nullptr;
    ctos_initialized_ = false;
}

void PacketSniffer::LogPacket(PacketLogEntry entry) {
    std::scoped_lock lock(mutex_);
    logs_.push_back(std::move(entry));
    while (logs_.size() > kMaxLogs) {
        logs_.erase(logs_.begin());
    }
}

std::vector<PacketLogEntry> PacketSniffer::GetLogs() {
    std::scoped_lock lock(mutex_);
    return logs_;
}

std::vector<PacketLogEntry> PacketSniffer::GetLogsByDirection(PacketDirection direction) {
    std::scoped_lock lock(mutex_);
    std::vector<PacketLogEntry> filtered;
    filtered.reserve(logs_.size());
    for (const auto& entry : logs_) {
        if (entry.direction == direction) {
            filtered.push_back(entry);
        }
    }
    return filtered;
}

void PacketSniffer::ClearLogs() {
    std::scoped_lock lock(mutex_);
    logs_.clear();
}

void PacketSniffer::ClearLogsByDirection(PacketDirection direction) {
    std::scoped_lock lock(mutex_);
    logs_.erase(
        std::remove_if(
            logs_.begin(),
            logs_.end(),
            [direction](const PacketLogEntry& entry) {
                return entry.direction == direction;
            }),
        logs_.end());
}

PYBIND11_EMBEDDED_MODULE(PyPacketSniffer, m) {
    m.doc() = R"doc(
Unified packet sniffer for Py4GW.

Captures raw StoC and CToS packets through a single PacketSniffer singleton.
)doc";

    py::enum_<PacketDirection>(m, "PacketDirection")
        .value("StoC", PacketDirection::StoC)
        .value("CToS", PacketDirection::CToS)
        .export_values();

    py::class_<PacketLogEntry>(m, "PacketLogEntry")
        .def(py::init<>())
        .def_readonly("tick", &PacketLogEntry::tick)
        .def_readonly("direction", &PacketLogEntry::direction)
        .def_readonly("header", &PacketLogEntry::header)
        .def_readonly("size", &PacketLogEntry::size)
        .def_readonly("data", &PacketLogEntry::data)
        .def("__repr__", [](const PacketLogEntry& entry) {
            const char* direction = entry.direction == PacketDirection::StoC ? "StoC" : "CToS";
            char buf[16];
            snprintf(buf, sizeof(buf), "%04X", entry.header);
            return "<Packet direction=" + std::string(direction) +
                " header=0x" + std::string(buf) +
                " size=" + std::to_string(entry.size) + ">";
        });

    py::class_<PacketSniffer>(m, "PacketSniffer")
        .def_static("instance", &PacketSniffer::Instance, py::return_value_policy::reference)
        .def("initialize", &PacketSniffer::Initialize)
        .def("initialize_stoc", &PacketSniffer::InitializeStoC)
        .def("initialize_ctos", &PacketSniffer::InitializeCToS)
        .def("terminate", &PacketSniffer::Terminate)
        .def("terminate_stoc", &PacketSniffer::TerminateStoC)
        .def("terminate_ctos", &PacketSniffer::TerminateCToS)
        .def("get_logs", &PacketSniffer::GetLogs)
        .def("get_stoc_logs", [](PacketSniffer& self) { return self.GetLogsByDirection(PacketDirection::StoC); })
        .def("get_ctos_logs", [](PacketSniffer& self) { return self.GetLogsByDirection(PacketDirection::CToS); })
        .def("clear_logs", &PacketSniffer::ClearLogs)
        .def("clear_stoc_logs", [](PacketSniffer& self) { self.ClearLogsByDirection(PacketDirection::StoC); })
        .def("clear_ctos_logs", [](PacketSniffer& self) { self.ClearLogsByDirection(PacketDirection::CToS); });
}
