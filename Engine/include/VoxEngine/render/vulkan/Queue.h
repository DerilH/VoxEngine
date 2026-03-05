//
// Created by deril on 2/13/26.
//

#pragma once

#include "QueueFamilyRepository.h"
#include "VulkanCommandBuffer.h"
#include "VoxCore/containers/ArrayView.h"
#include "VulkanObject.h"

VULKAN_NS
    class Queue : public VulkanObject<VkQueue> {
        QueueFamily mFamily;

    public:
        Queue(VkQueue handle, QueueFamily family);

        void submit(const ArrayView<VkCommandBuffer> &cmdBuffers, bool wait) const;
        void submit(const ArrayView<VkCommandBuffer> &cmdBuffers, const ArrayView<VkSemaphore> &waitSemaphores, const ArrayView<VkSemaphore> &finishSemaphore, const ArrayView<VkPipelineStageFlags> & waitStages, VkFence fence = VK_NULL_HANDLE) const;

        QueueFamily getFamily() const;
    };
NS_END