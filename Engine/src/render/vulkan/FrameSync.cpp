//
// Created by deril on 2/16/26.
//

#include <VoxEngine/render/vulkan/FrameSync.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/VulkanTypes.h>

VULKAN_NS
    FrameSync::FrameSync(const VulkanDevice &device, Fence fence, Semaphore imageSemaphore, CommandBufferRef buffer) : mDevice(device), mFence(std::move(fence)), mImageWaitSemaphore(std::move(imageSemaphore)), mCommandBuffer(buffer) {
    }

    FrameSync FrameSync::Create(const VulkanDevice &device) {
        const Fence fence = device.create<Fence>();
        const Semaphore imageSemaphore = device.create<Semaphore>();
        CommandBufferRef buffer = device.getCmdPool(GRAPHICS_QUEUE).allocBuffer();
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
                mStarted = false;
                return -1;
            } else { VK_CHECK(res, "Failed to acquire swapchain image")}
        }

        mCurrentImageIndex = imgIndex;
        return imgIndex;
    }

    void FrameSync::submit() {
        VOX_CHECK(mStarted, "Frame not started");
        VOX_CHECK(mRenderWaitSemaphore.has_value(), "Image wait semaphore not set")

        mDevice.getQueue(GRAPHICS_QUEUE).submit({ResourceCast(mCommandBuffer)->getHandle()}, {mImageWaitSemaphore}, {mRenderWaitSemaphore.value()}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }, mFence);
        mStarted = false;
    }
NS_END
