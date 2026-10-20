//
// Created by deril on 2/20/26.
//

#pragma once
#include <cstdint>
#include <string_view>
#include <magic_enum/magic_enum.hpp>
#include <VoxCore/Define.h>

RESOURCES_NS
    enum class ResourceType : uint16_t {
        UNKNOWN = 0,
        MESH,
        MODEL,
        MATERIAL,
        SHADER,
        TEXTURE
    };

    constexpr std::string_view operator+(const ResourceType type) noexcept {
        return magic_enum::enum_name(type);
    }
NS_END
