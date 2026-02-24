//
// Created by deril on 2/13/26.
//

#pragma once

#include "QueueFamilyRepository.h"
#include "CommandBuffer.h"
#include "VoxCore/containers/Buffer.h"
#include "VulkanObject.h"

VULKAN_NS
    class Queue : public VulkanObject<VkQueue> {
        QueueFamily mFamily;

    public:
        Queue(VkQueue handle, QueueFamily family);

        void submit(const Buffer<VkCommandBuffer> &cmdBuffers, bool wait) const;
        void submit(const Buffer<VkCommandBuffer> &cmdBuffers, const Buffer<VkSemaphore> &waitSemaphores, const Buffer<VkSemaphore> &finishSemaphore, const Buffer<VkPipelineStageFlags> & waitStages, VkFence fence = VK_NULL_HANDLE) const;

        QueueFamily getFamily() const;
    };
NS_END