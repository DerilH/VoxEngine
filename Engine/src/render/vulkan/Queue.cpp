//
// Created by deril on 2/13/26.
//

#include <VoxEngine/render/vulkan/Queue.h>
#include "VoxEngine/render/vulkan/CommandBuffer.h"
#include "VoxCore/containers/Buffer.h"

namespace Vox::Render::Vulkan {
    Queue::Queue(VkQueue handle, const QueueFamily family) : mHandle(handle), mFamily(family) {
    }

    VkQueue Queue::getHandle() const {
        return mHandle;
    }

    QueueFamily Queue::getFamily() const {
        return mFamily;
    }

    void Queue::submit(const Buffer <VkCommandBuffer> &cmdBuffers, bool wait) const {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = cmdBuffers.size;
        submitInfo.pCommandBuffers = cmdBuffers.pData;

        VK_CHECK(vkQueueSubmit(mHandle, 1, &submitInfo, VK_NULL_HANDLE), "Queue submit error");
        if(wait) {
            vkQueueWaitIdle(mHandle);
        }
    }

    void Queue::submit(Buffer, S) const {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        VkCommandBuffer buffers[] = {buffer.getHandle()};
        submitInfo.pCommandBuffers = buffers;

        VK_CHECK(vkQueueSubmit(mHandle, 1, &submitInfo, VK_NULL_HANDLE), "Queue submit error");
        if(wait) {
            vkQueueWaitIdle(mHandle);
        }
    }
}
