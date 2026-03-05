//
// Created by deril on 2/13/26.
//

#include <VoxEngine/render/vulkan/Queue.h>
#include "VoxEngine/render/vulkan/VulkanCommandBuffer.h"
#include "VoxCore/containers/ArrayView.h"

namespace Vox::Render::Vulkan {
    Queue::Queue(VkQueue handle, const QueueFamily family) : VulkanObject(handle), mFamily(family) {
    }

    QueueFamily Queue::getFamily() const {
        return mFamily;
    }

    void Queue::submit(const ArrayView <VkCommandBuffer> &cmdBuffers, bool wait) const {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = cmdBuffers.size();
        submitInfo.pCommandBuffers = cmdBuffers.pData;
        submitInfo.waitSemaphoreCount = 0;


        VK_CHECK(vkQueueSubmit(mHandle, 1, &submitInfo, VK_NULL_HANDLE), "Queue submit error");
        if(wait) {
            vkQueueWaitIdle(mHandle);
        }
    }

    void Queue::submit(const ArrayView<VkCommandBuffer> &cmdBuffers, const ArrayView<VkSemaphore> &waitSemaphores, const ArrayView<VkSemaphore> &finishSemaphores, const ArrayView<VkPipelineStageFlags> & waitStages, VkFence fence) const {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = cmdBuffers.size();
        submitInfo.pCommandBuffers = cmdBuffers.pData;

        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores = waitSemaphores.pData;
        submitInfo.pWaitDstStageMask = waitStages.pData;

        submitInfo.signalSemaphoreCount = finishSemaphores.size();
        submitInfo.pSignalSemaphores = finishSemaphores.pData;

        VK_CHECK(vkQueueSubmit(mHandle, 1, &submitInfo, fence), "Queue submit error");
    }
}
