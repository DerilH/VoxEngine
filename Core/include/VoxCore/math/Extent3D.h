//
// Created by deril on 2/28/26.
//

#pragma once

#include <cstdint>
#include "VoxCore/Define.h"

VOX_NS
struct Extent3D {
    const uint32_t width;
    const uint32_t height;
    const uint32_t depth;
    Extent3D(uint32_t width, uint32_t height, uint32_t depth) : width(width), height(height), depth(depth) {}
    Extent3D() : width(0), height(0), depth(0) {};
};
NS_END
