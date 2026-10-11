//
// Created by deril on 2/16/26.
//

#include <VoxEngine/render/vulkan/SwapChainFrameSync.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>
#include <VoxEngine/render/vulkan/VulkanUtil.h>

VULKAN_NS
    SwapChainFrameSync::SwapChainFrameSync(const VulkanDevice &device, Ref<SwapChain> swapChain, Fence fence, Semaphore imageSemaphore, CommandBufferRef buffer, VulkanDescriptorPoolRef pool) : mDevice(device), mSwapChain(swapChain), mFence(fence), mImageWaitSemaphore(imageSemaphore), mCommandBuffer(buffer), mDescriptorPool(pool) {
    }

    SwapChainFrameSync SwapChainFrameSync::Create(const VulkanDevice &device, Ref<SwapChain> swapChain) {
        const auto fence = device.create<Fence>();
        const auto imageSemaphore = device.create<Semaphore>();
        CommandBufferRef buffer = device.getCmdPool(GRAPHICS_QUEUE).allocBuffer();
        auto pool = device.createHeap<VulkanDescriptorPool>(4, ArrayView<VkDescriptorPoolSize>{{toVk(ShaderResourceType::UNIFORM_BUFFER), 100}});
        return {device, swapChain, fence, imageSemaphore, buffer, pool};
    }

    uint32_t SwapChainFrameSync::begin() {
        VOX_ASSERT(!mStarted, "Frame already started");
        mStarted = true;

        VkFence fence = mFence.getHandle();
        vkWaitForFences(mDevice.getHandle(), 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(mDevice.getHandle(), 1, &fence);

        uint32_t imgIndex = 0;
        VkResult res = mSwapChain->acquireNextImage(mImageWaitSemaphore, &imgIndex);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
            mSwapChain->markForRebuild();
            mStarted = false;
            return -1;
        } else { VK_CHECK(res, "Failed to acquire swapchain image") }

        mCurrentImageIndex = imgIndex;
        return imgIndex;
    }

    void SwapChainFrameSync::submit() {
        VOX_CHECK(mStarted, "Frame not started");
        VOX_CHECK(mRenderWaitSemaphore.has_value(), "Image wait semaphore not set")

        mDevice.getQueue(GRAPHICS_QUEUE).submit({ResourceCast(mCommandBuffer)->getHandle()}, {mImageWaitSemaphore.getHandle()}, {mRenderWaitSemaphore.value().getHandle()}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, mFence.getHandle());
        mStarted = false;
    }

NS_END
