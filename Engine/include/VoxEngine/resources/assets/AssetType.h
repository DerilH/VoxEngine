//
// Created by deril on 2/20/26.
//

#pragma once
#include <cstdint>
#include <magic_enum/magic_enum.hpp>

enum class AssetType : uint16_t {
    REGULAR,
    MODEL,
    MESH,
    MATERIAL,
    SHADER,
    TEXTURE
};

constexpr std::string_view operator+(const AssetType type) noexcept {
    return magic_enum::enum_name(type);
}