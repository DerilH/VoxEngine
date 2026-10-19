//
// Created by deril on 2/28/26.
//

#pragma once

#include <cstdint>
#include "VoxCore/Define.h"

VOX_NS
struct alignas(4) Extent {
    uint32_t width;
    uint32_t height;
    constexpr Extent(uint32_t width, uint32_t height) : width(width), height(height) {}
    constexpr Extent() : width(0), height(0) {};

    constexpr bool operator==(const Extent & size) const {
        return width == size.width && height == size.height;
    }
};
NS_END
