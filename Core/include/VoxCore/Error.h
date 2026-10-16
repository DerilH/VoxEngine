#pragma once
#include "containers/Containers.h"

namespace Vox {
    // class Error {
    //     static constexpr size_t kMaxArgs = 4;
    //
    //     const char* mMessage = nullptr;
    //     // std::array<InternedString, kMaxArgs> mArgs{};
    //     // uint8_t mCount = 0;
    //
    // public:
    //     template <std::same_as<InternedString>... Args>
    //         requires (sizeof...(Args) <= kMaxArgs)
    //     Error(const char* message, Args... args)
    //         : mMessage(message)
    //         , mArgs{args...}
    //     , mCount(sizeof...(Args)) {}
    // };

    struct Error {
        const char* message;
        Error(const char* msg) : message(msg) {}
    };
}
