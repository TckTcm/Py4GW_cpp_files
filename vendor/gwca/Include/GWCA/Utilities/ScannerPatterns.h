#pragma once

#include <cstddef>

namespace GW::Scanner::Patterns {
    struct CallingPattern {
        const char* bytes;
        const char* mask;
        size_t call_offset;
    };

    namespace AgentMgr {
        inline constexpr char ChangeTargetManualWrapperBytes[] =
            "\x55\x8B\xEC\xFF\x35\x00\x00\x00\x00\xFF\x75\x08\xE8\x00\x00\x00\x00\x83\xC4\x08\x5D\xC3";
        inline constexpr char ChangeTargetManualWrapperMask[] =
            "xxxxx????xxxx????xxxxx";

        static_assert(
            sizeof(ChangeTargetManualWrapperBytes) == sizeof(ChangeTargetManualWrapperMask),
            "ChangeTargetManualWrapper mask must match the byte pattern length"
        );

        inline constexpr CallingPattern ChangeTargetManualWrapper = {
            ChangeTargetManualWrapperBytes,
            ChangeTargetManualWrapperMask,
            0xC
        };
    }
}
