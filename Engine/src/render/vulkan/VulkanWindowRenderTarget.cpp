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
        mExtent = mSurface.getSwapChain().getExtent();

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

    VkFramebuffer VulkanWindowRenderTarget::getFramebuffer(const RenderPassType type) const {
        VOX_NO_IMPL("getFrameBuffer");
        return mSurface.getSwapChain()[type, mFrames[mCurrentFrame].getCurrentImageIndex()];
    }

    RenderPass & VulkanWindowRenderTarget::getRenderPass(RenderPassType type) const {
        VOX_NO_IMPL("getRenderPass");
        return *mRenderPasses.at(type);
    }

    void VulkanWindowRenderTarget::addRenderPass(const RenderPassType type) {
        VOX_NO_IMPL("getRenderPass");
        auto* pass = mDevice->createHeap<RenderPass>(getFormat(), type);
        mSurface.getSwapChain().addRenderPass(*pass);
        mRenderPasses.emplace(type,pass);
    }

    VkFormat VulkanWindowRenderTarget::getFormat() const {
        return mSurface.getCurrentFormat().format;
    }

    VkImageView VulkanWindowRenderTarget::getImageView() const {
        return mSurface.getSwapChain()[mFrames[mCurrentFrame].getCurrentImageIndex()];
    }
    VkImage VulkanWindowRenderTarget::getImage() const {
        return mSurface.getSwapChain().getImage(mFrames[mCurrentFrame].getCurrentImageIndex());
    }

NS_END
