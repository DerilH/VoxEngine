//
// Created by deril on 2/16/26.
//

#include <VoxEngine/render/vulkan/FrameSync.h>
#include <VoxEngine/render/vulkan/LogicalDevice.h>

VULKAN_NS
    FrameSync::FrameSync(const LogicalDevice &device, Fence fence, Semaphore imageSemaphore, const CommandBuffer buffer) : mDevice(device), mFence(std::move(fence)), mImageWaitSemaphore(std::move(imageSemaphore)), mCommandBuffer(buffer) {
    }

    FrameSync FrameSync::Create(const LogicalDevice &device) {
        const Fence fence = device.create<Fence>();
        const Semaphore imageSemaphore = device.create<Semaphore>();
        const CommandBuffer buffer = device.getCmdPool(GRAPHICS_QUEUE).allocateBuffer();
        return {device, fence, imageSemaphore, buffer};
    }

    uint32_t FrameSync::begin(std::optional<std::reference_wrapper<SwapChain>> swapChain) {
        VOX_ASSERT(!mStarted, "Frame already started");
        mStarted = true;

        VkFence fence = mFence.getHandle();
        vkWaitForFences(mDevice.getHandle(), 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(mDevice.getHandle(), 1, &fence);

        uint32_t imgIndex = 0;
        if (swapChain.has_value()) {
            VkResult res = swapChain->get().acquireNextImage(mImageWaitSemaphore, &imgIndex);
            if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
                swapChain->get().markForRebuild();
                return -1;
            } else VK_CHECK(res, "Failed to acquire swapchain image")
        }

        mCurrentImageIndex = imgIndex;
        return imgIndex;
    }

    void FrameSync::submit() {
        VOX_CHECK(mStarted, "Frame not started");
        VOX_CHECK(mRenderWaitSemaphore.has_value(), "Image wait semaphore not set")

        static VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        const VkSemaphore waitSemaphores[] = {mImageWaitSemaphore.getHandle()};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        VkCommandBuffer bfs = {mCommandBuffer.getHandle()};
        submitInfo.pCommandBuffers = &bfs;

        const VkSemaphore signalSemaphores[] = {mRenderWaitSemaphore.value().getHandle()};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        mDevice.getQueue(GRAPHICS_QUEUE).submit(mCommandBuffer, true);
//        VK_CHECK(vkQueueSubmit(mDevice.getQueue(GRAPHICS_QUEUE).getHandle(), 1, &submitInfo, mFence.getHandle()), "failed to submit draw command buffers");
        mStarted = false;
    }

    const Semaphore &FrameSync::getRenderWaitSemaphore() const {
        return mRenderWaitSemaphore.value();
    }

    const Semaphore &FrameSync::getImageWaitSemaphore() const {
        return mImageWaitSemaphore;
    }

    CommandBuffer FrameSync::getCmdBuffer() const {
        return mCommandBuffer;
    }

    uint32_t FrameSync::getCurrentImageIndex() const {
        return mCurrentImageIndex;
    }

    void FrameSync::setRenderWaitSemaphore(const Semaphore &semaphore) {
        mRenderWaitSemaphore = semaphore;
    }

NS_END
