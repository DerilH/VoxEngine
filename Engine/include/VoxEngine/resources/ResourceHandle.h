//
// Created by deril on 2/20/26.
//

#pragma once
#include <cstdint>
#include <format>
#include <string>
#include <VoxCore/Define.h>
#include <VoxCore/Assert.h>
#include "ResourceType.h"

RESOURCES_NS
    template<typename T>
    struct ResourceTraits;

    struct ResourceHandleRaw {
        uint32_t index{0};
        uint16_t generation{0};
        ResourceType type{ResourceType::UNKNOWN};

        constexpr ResourceHandleRaw() = default;
        constexpr ResourceHandleRaw(uint32_t index, uint16_t generation, ResourceType type)
            : index(index), generation(generation), type(type) {}

        constexpr bool isValid() const noexcept {
            return generation != 0 && type != ResourceType::UNKNOWN;
        }

        constexpr explicit operator bool() const noexcept {
            return isValid();
        }

        constexpr friend bool operator==(const ResourceHandleRaw &a, const ResourceHandleRaw &b) = default;

        constexpr uint64_t toUint64() const noexcept {
            return (static_cast<uint64_t>(index) << 32) |
                   (static_cast<uint64_t>(generation) << 16) |
                   static_cast<uint64_t>(type);
        }

        static constexpr ResourceHandleRaw fromUint64(uint64_t val) noexcept {
            return ResourceHandleRaw(
                static_cast<uint32_t>(val >> 32),
                static_cast<uint16_t>((val >> 16) & 0xFFFF),
                static_cast<ResourceType>(val & 0xFFFF)
            );
        }

        std::string toString() const {
            return std::format("{}:{}:{}", +type, index, generation);
        }
    };

    template<typename T>
    struct ResourceHandle {
        uint32_t index{0};
        uint16_t generation{0};
        ResourceType type{ResourceType::UNKNOWN};

        constexpr ResourceHandle()
            : index(0), generation(0), type(ResourceTraits<T>::Type) {}

        constexpr ResourceHandle(uint32_t index, uint16_t generation)
            : index(index), generation(generation), type(ResourceTraits<T>::Type) {}

        explicit ResourceHandle(ResourceHandleRaw raw)
            : index(raw.index), generation(raw.generation), type(raw.type) {
            VOX_ASSERT(raw.type == ResourceTraits<T>::Type || !raw.isValid(), "ResourceHandle type mismatch");
        }

        constexpr bool isValid() const noexcept {
            return generation != 0 && type == ResourceTraits<T>::Type;
        }

        constexpr explicit operator bool() const noexcept {
            return isValid();
        }

        constexpr operator ResourceHandleRaw() const noexcept {
            return ResourceHandleRaw(index, generation, type);
        }

        constexpr friend bool operator==(const ResourceHandle &a, const ResourceHandle &b) = default;
        constexpr friend bool operator==(const ResourceHandle &a, const ResourceHandleRaw &b) noexcept {
            return ResourceHandleRaw(a) == b;
        }
        constexpr friend bool operator==(const ResourceHandleRaw &a, const ResourceHandle &b) noexcept {
            return a == ResourceHandleRaw(b);
        }

        std::string toString() const {
            return std::format("{}:{}:{}", +type, index, generation);
        }
    };
NS_END

template<>
struct std::hash<Vox::Resources::ResourceHandleRaw> {
    size_t operator()(const Vox::Resources::ResourceHandleRaw &h) const noexcept {
        return std::hash<uint64_t>{}(h.toUint64());
    }
};

template<typename T>
struct std::hash<Vox::Resources::ResourceHandle<T>> {
    size_t operator()(const Vox::Resources::ResourceHandle<T> &h) const noexcept {
        return std::hash<uint64_t>{}(static_cast<Vox::Resources::ResourceHandleRaw>(h).toUint64());
    }
};

template<>
struct std::formatter<Vox::Resources::ResourceHandleRaw> : std::formatter<std::string> {
    auto format(const Vox::Resources::ResourceHandleRaw &h, std::format_context &ctx) const {
        return std::formatter<std::string>::format(h.toString(), ctx);
    }
};

template<typename T>
struct std::formatter<Vox::Resources::ResourceHandle<T>> : std::formatter<std::string> {
    auto format(const Vox::Resources::ResourceHandle<T> &h, std::format_context &ctx) const {
        return std::formatter<std::string>::format(h.toString(), ctx);
    }
};
