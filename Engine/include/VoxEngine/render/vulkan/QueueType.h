//
// Created by deril on 2/13/26.
//

#pragma once
#include <vulkan/vulkan_core.h>

namespace Vox::Render::Vulkan {
    typedef enum {
        GRAPHICS_QUEUE = VK_QUEUE_GRAPHICS_BIT,
        TRANSFER_QUEUE = VK_QUEUE_TRANSFER_BIT,
    } QueueType;
}
