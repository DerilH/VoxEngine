//
// Created by deril on 2/13/26.
//

#pragma once
#include <vulkan/vulkan_core.h>

VULKAN_NS
    typedef enum {
        GRAPHICS_QUEUE = VK_QUEUE_GRAPHICS_BIT,
        TRANSFER_QUEUE = VK_QUEUE_TRANSFER_BIT,
    } QueueType;
NS_END