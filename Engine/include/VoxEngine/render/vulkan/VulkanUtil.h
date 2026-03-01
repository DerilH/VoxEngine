//
// Created by deril on 2/28/26.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include "VoxCore/Define.h"
#include "VoxCore/math/Extent.h"
#include "VoxCore/math/Extent3D.h"

VULKAN_NS
    inline VkExtent2D toVk(Extent& extent) { return {extent.width, extent.height}; }

    inline VkExtent2D toVk(Extent&& extent) { return {extent.width, extent.height}; }

    inline VkExtent3D toVk(Extent3D& extent) { return {extent.width, extent.height, extent.depth}; }

    inline VkExtent3D toVk(Extent3D&& extent) { return {extent.width, extent.height, extent.depth}; }
NS_END
