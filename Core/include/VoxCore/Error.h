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
        InternedString args[4]{};
        uint8_t argCount{0};

        explicit Error(const char* msg)
            : message(msg)
        {}

        template <typename... Args>
        Error(const char* msg, Args&&... inputArgs)
            : message(msg)
        {
            static_assert(sizeof...(Args) <= 4, "Error structure supports a maximum of 4 arguments");

            InternedString tempArgs[]{ std::forward<Args>(inputArgs)... };
            for (size_t i = 0; i < sizeof...(Args); ++i) {
                args[i] = tempArgs[i];
            }
            argCount = static_cast<uint8_t>(sizeof...(Args));
        }

        std::string format() const {
        if (!message) return {};

        std::string result;
        result.reserve(128);

        std::string_view fmt(message);
        size_t autoIdx = 0;
        size_t pos = 0;

        while (pos < fmt.length()) {
            size_t openBrace = fmt.find('{', pos);

            if (openBrace == std::string_view::npos) {
                result.append(fmt.substr(pos));
                break;
            }

            size_t closeBrace = fmt.find('}', openBrace);
            if (closeBrace == std::string_view::npos) {
                result.append(fmt.substr(pos));
                break;
            }

            // Добавляем текст ДО открывающей скобки
            result.append(fmt.substr(pos, openBrace - pos));

            std::string_view inside = fmt.substr(openBrace + 1, closeBrace - openBrace - 1);

            size_t targetIdx = 0;

            if (inside.empty()) {
                targetIdx = autoIdx++;
            } else {
                targetIdx = 0;
                for (char c : inside) {
                    if (c >= '0' && c <= '9') {
                        targetIdx = targetIdx * 10 + (c - '0');
                    }
                }
            }

            if (targetIdx < argCount && targetIdx < 4) {
                if (const char* argStr = args[targetIdx].c_str()) {
                    result.append(argStr);
                }
            } else {
                result.append(fmt.substr(openBrace, closeBrace - openBrace + 1));
            }

            pos = closeBrace + 1;
        }

        return result;
    }
    };
}
