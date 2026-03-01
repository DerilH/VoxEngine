//
// Created by deril on 2/17/26.
//

#include <VoxEngine/render/vulkan/LogicalDevice.h>
#include <VoxEngine/render/vulkan/VulkanRenderTarget.h>

VULKAN_NS
    void VulkanRenderTarget::createFrames(const char buffers) {
        mFrames.clear();
        mFrames.reserve(buffers);
        mRenderFinishedSemaphores.reserve(buffers);
        for (int i = 0; i < buffers; i++) {
            mFrames.emplace_back(mDevice->create<FrameSync>());
            mRenderFinishedSemaphores.emplace_back(mDevice->create<Semaphore>());
        }
    }

    void VulkanRenderTarget::nextFrame() {
        mCurrentFrame = (mCurrentFrame + 1) % mFrames.size();
    }

    VulkanRenderTarget::VulkanRenderTarget(const LogicalDevice *device, const VkExtent2D extent) : mDevice(device), mExtent({extent.width, extent.height}) {
    }

    void VulkanRenderTarget::submit() {
        mFrames[mCurrentFrame].submit();
    }

    void VulkanRenderTarget::setBuffering(char buffers) {
    }

    const FrameSync &VulkanRenderTarget::getCurrentFrame() const {
        return mFrames[mCurrentFrame];
    }

    Extent VulkanRenderTarget::getExtent() const {
        return mExtent;
    }

NS_END
