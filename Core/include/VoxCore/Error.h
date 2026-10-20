#pragma once
#include "containers/Containers.h"
#include <format>
#include <string_view>
#include <type_traits>

namespace Vox {
    namespace Detail {
        template <typename T>
        InternedString toErrorArg(T&& val) {
            using Decayed = std::decay_t<T>;
            if constexpr (std::is_same_v<Decayed, InternedString>) {
                return std::forward<T>(val);
            } else if constexpr (std::is_convertible_v<T, std::string_view>) {
                return InternedString(std::string_view(val));
            } else if constexpr (std::is_arithmetic_v<Decayed>) {
                return std::to_string(val);
            } else {
                return std::format("{}", val);
            }
        }
    }

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

            InternedString tempArgs[]{ Detail::toErrorArg(std::forward<Args>(inputArgs))... };
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
