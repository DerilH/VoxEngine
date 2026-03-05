//
// Created by deril on 2/23/26.
//

#pragma once


#include "VulkanObject.h"
#include "VoxEngine/render/CommandBuffer.h"

VULKAN_NS
    class VulkanCommandBuffer : public CommandBuffer, public VulkanObject<VkCommandBuffer> {
    public:
    };
NS_END
