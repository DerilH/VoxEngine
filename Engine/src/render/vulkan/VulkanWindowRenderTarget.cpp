//
// Created by deril on 2/17/26.
//

#include <VoxEngine/render/vulkan/LogicalDevice.h>
#include <VoxEngine/render/vulkan/Surface.h>
#include <VoxEngine/render/vulkan/VulkanWindowRenderTarget.h>

VULKAN_NS

    VulkanWindowRenderTarget::VulkanWindowRenderTarget(Surface &surface) : VulkanRenderTarget(surface.getCurrentDevice(), surface.getSwapChain().getExtent()), mSurface(surface) {
        createFrames(surface.getSwapChain().getImageCount());
    }

    bool VulkanWindowRenderTarget::begin() {
        mSurface.update();
        auto e = mSurface.getSwapChain().getExtent();
        mExtent = {e.width, e.height};

        FrameSync& frame = mFrames[mCurrentFrame];
        const uint32_t index = frame.begin(mSurface.getSwapChain());
        if (index == -1) {
            LOG_VERBOSE("Swapchain rebuild needed");
            return false;
        }

        frame.setRenderWaitSemaphore(mRenderFinishedSemaphores[index]);
        return true;
    }

    void VulkanWindowRenderTarget::present() {
        mSurface.presentFrame(mFrames[mCurrentFrame]);
        nextFrame();
    }

    Format VulkanWindowRenderTarget::getFormat() const {
        return {mSurface.getCurrentFormat().format};
    }

    VkImageView VulkanWindowRenderTarget::getImageView() const {
        return mSurface.getSwapChain()[mFrames[mCurrentFrame].getCurrentImageIndex()];
    }
    VkImage VulkanWindowRenderTarget::getImage() const {
        return mSurface.getSwapChain().getImage(mFrames[mCurrentFrame].getCurrentImageIndex());
    }

NS_END
