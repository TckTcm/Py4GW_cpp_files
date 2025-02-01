#include "Headers.h"

namespace py = pybind11;


class PingTracker {
public:
    PingTracker(size_t history_size = 10)
        : ping_history(history_size, 0), ping_index(0), history_size(history_size), ping_count(0), is_initialized(false) {
        Initialize();  // Automatically initialize the callback
    }

    ~PingTracker() {
        Terminate();
    }


    // Initialize the callback
    void Initialize() {
        if (!is_initialized) {
            GW::StoC::RegisterPacketCallback(&ping_callback, GAME_SMSG_PING_REPLY,
                [this](GW::HookStatus*, void* packet) {
                    OnPingReceived(packet);  // Call the instance-specific handler
                }
            );
            is_initialized = true;
        }
    }

    // Terminate the callback (should be called manually from Python)
    void Terminate() {
        if (is_initialized) {
            GW::StoC::RemoveCallback(GAME_SMSG_PING_REPLY, &ping_callback);
            is_initialized = false;
        }
    }

    // Returns the current ping (the last one recorded)
    uint32_t GetCurrentPing() const {
        if (ping_count == 0) return 0;
        return ping_history[(ping_index + history_size - 1) % history_size];
    }

    uint32_t GetAveragePing() const {
        if (ping_count == 0) return 0;
        uint32_t sum = 0;
        for (size_t i = 0; i < ping_count; ++i) {
            sum += ping_history[i];
        }
        return sum / ping_count;
    }


    uint32_t GetMinPing() const {
        if (ping_count == 0) return 0;
        return *std::min_element(ping_history.begin(), ping_history.begin() + ping_count);
    }

    uint32_t GetMaxPing() const {
        if (ping_count == 0) return 0;
        return *std::max_element(ping_history.begin(), ping_history.begin() + ping_count);
    }


private:
    std::vector<uint32_t> ping_history;  // Stores ping history
    size_t ping_index;                   // Current index in history
    size_t history_size;                 // Size of the history buffer
    size_t ping_count;                   // Number of pings recorded
    bool is_initialized;                 // Tracks if callback is initialized

    GW::HookEntry ping_callback;  // Callback hook entry

    void OnPingReceived(void* packet) {
        const auto packet_data = static_cast<uint32_t*>(packet);
        const uint32_t ping = packet_data[1];

        if (ping == 0 || ping > 4999) return;  // Ignore invalid pings

        // Update the ping history for this instance
        ping_history[ping_index] = ping;
        ping_index = (ping_index + 1) % history_size;

        if (ping_count < history_size) {
            ping_count++;
        }
    }
};

