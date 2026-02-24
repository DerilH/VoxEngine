//
// Created by deril on 2/13/26.
//

#include <VoxEngine/render/vulkan/Queue.h>
#include "VoxEngine/render/vulkan/CommandBuffer.h"
#include "VoxCore/containers/Buffer.h"

namespace Vox::Render::Vulkan {
    Queue::Queue(VkQueue handle, const QueueFamily family) : VulkanObject(handle), mFamily(family) {
    }

    QueueFamily Queue::getFamily() const {
        return mFamily;
    }

    void Queue::submit(const Buffer <VkCommandBuffer> &cmdBuffers, bool wait) const {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = cmdBuffers.size;
        submitInfo.pCommandBuffers = cmdBuffers.pData;
        submitInfo.waitSemaphoreCount = 0;


        VK_CHECK(vkQueueSubmit(mHandle, 1, &submitInfo, VK_NULL_HANDLE), "Queue submit error");
        if(wait) {
            vkQueueWaitIdle(mHandle);
        }
    }

    void Queue::submit(const Buffer<VkCommandBuffer> &cmdBuffers, const Buffer<VkSemaphore> &waitSemaphores, const Buffer<VkSemaphore> &finishSemaphores, const Buffer<VkPipelineStageFlags> & waitStages, VkFence fence) const {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = cmdBuffers.size;
        submitInfo.pCommandBuffers = cmdBuffers.pData;

        submitInfo.waitSemaphoreCount = waitSemaphores.size;
        submitInfo.pWaitSemaphores = waitSemaphores.pData;
        submitInfo.pWaitDstStageMask = waitStages.pData;

        submitInfo.signalSemaphoreCount = finishSemaphores.size;
        submitInfo.pSignalSemaphores = finishSemaphores.pData;

        VK_CHECK(vkQueueSubmit(mHandle, 1, &submitInfo, fence), "Queue submit error");
    }
}
