//
// Created by deril on 2/13/26.
//


#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/Surface.h>
#include <VoxEngine/render/vulkan/VulkanFrameSync.h>
#include <VoxEngine/render/vulkan/SwapChain.h>
#include "VoxEngine/render/vulkan/VulkanUtil.h"

namespace Vox::Render::Vulkan {
    SwapChainSupportDetails Surface::querySwapChainSupport() const {
        VkSurfaceCapabilitiesKHR capabilities;

        VkPhysicalDevice device = mDevice->getPhysicalDevice().getHandle();
        VkSurfaceKHR surface = mHandle;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        if (formatCount != 0) {
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        if (presentModeCount != 0) {
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.data());
        }

        return SwapChainSupportDetails{capabilities, formats, presentModes};
    }

    std::optional<QueueFamily> Surface::findPresentFamily() const {
        for (const auto& family: mDevice->getPhysicalDevice().getQueueFamilies().getUniqueFamilies()) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(mDevice->getPhysicalDevice().getHandle(), family.index(), mHandle, &presentSupport);

            if (presentSupport) {
                return family;
            }
        }
        return std::nullopt;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat: availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }


    void Surface::setDevice(VulkanDevice* device) {
        mDevice = device;
        const std::optional<QueueFamily> family = findPresentFamily();
        VOX_CHECK(family.has_value(), "Device cant be used for present");
        mPresentQueue = &(device->getQueues().at(family->type()));
        mSurfaceFormat = chooseSwapSurfaceFormat(querySwapChainSupport().formats());
        mImageFormat = fromVk(mSurfaceFormat.format);
        createSwapChain();
    }

    SwapChainSupportDetails::SwapChainSupportDetails(const VkSurfaceCapabilitiesKHR& capabilities,
                                                     const Vector<VkSurfaceFormatKHR>& formats,
                                                     const Vector<VkPresentModeKHR>& presentModes) : mCapabilities(
            capabilities), mFormats(formats), mPresentModes(presentModes) {
    }

    Surface::Surface(Extent extent, VkSurfaceKHR handle, void* windowHandle) : WindowRenderTarget(extent, windowHandle), mWindow(windowHandle), VulkanObject(handle) {
    }

    Surface* Surface::Create(Extent extent, VkInstance instance, void* window) {
        VkSurfaceKHR s = nullptr;

        VK_CHECK(glfwCreateWindowSurface(instance, (GLFWwindow*) window, nullptr, &s),
                 "failed to create window surface!");
        return new Surface{extent, s, window};
    }

    void Surface::update() {
        if (mCurrentSwapChain == nullptr || mCurrentSwapChain->needsRebuild()) {
            createSwapChain();
        }
    }

    void Surface::createSwapChain() {
        SwapChain* old = nullptr;
        if (mCurrentSwapChain != nullptr) {
            old = mCurrentSwapChain;
        }
        mCurrentSwapChain = SwapChain::Create(*this, old == nullptr ? nullptr : old->getHandle());
        createFrames(mCurrentSwapChain->getImageCount());
    }

    SwapChain& Surface::getSwapChain() const {
        return *mCurrentSwapChain;
    }

    Format Surface::getImageFormat() const {
        return mImageFormat;
    }

    VkSurfaceFormatKHR Surface::getSurfaceFormat() const {
        return mSurfaceFormat;
    }

    const VulkanDevice* Surface::getDevice() const {
        return mDevice;
    }

    const Queue* Surface::getPresentQueue() const {
        return mPresentQueue;
    }

    void Surface::presentFrame(const VulkanFrameSync& frame) const {
        VOX_ASSERT(mPresentQueue != nullptr, "No queue provided for present")
        VOX_ASSERT(mCurrentSwapChain != nullptr, "No swapchain provided for present")

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        const VkSemaphore s[] = {frame.getRenderWaitSemaphore()};
        presentInfo.pWaitSemaphores = s;

        presentInfo.swapchainCount = 1;
        const VkSwapchainKHR swapChains[] = {mCurrentSwapChain->getHandle()};
        presentInfo.pSwapchains = swapChains;

        const uint32_t index = frame.getCurrentImageIndex();
        presentInfo.pImageIndices = &index;

        VK_CHECK(vkQueuePresentKHR(mPresentQueue->getHandle(), &presentInfo), "Cannot present frame");
    }

    int32_t Surface::beginFrame() {
        this->update();
        auto& e = this->getSwapChain();
        mExtent = {e.getExtent().width, e.getExtent().height};

        auto frame = mFrames[mCurrentFrame];
        const uint32_t index = frame->begin(e);
        if (index == -1) {
            LOG_VERBOSE("Swapchain rebuild needed");
            return -1;
        }
        frame->setRenderWaitSemaphore(*mRenderFinishedSemaphores[index]);
        return 0;
    }

    void Surface::endFrame() {
        mFrames[mCurrentFrame]->submit();
        presentFrame(*mFrames[mCurrentFrame]);
        mCurrentFrame = (mCurrentFrame + 1) % mFrames.size();
    }

    TextureRef Surface::getBackBuffer() {
        return mCurrentSwapChain->getTexture(mFrames[mCurrentFrame]->getCurrentImageIndex());
    }

    void Surface::resize(Extent extent) {
        VOX_NO_IMPL("Resize not implemented")
    }

    void Surface::createFrames(const uint8_t buffers) {
        mFrames.clear();
        mFrames.reserve(buffers);
        mRenderFinishedSemaphores.reserve(buffers);
        for (int i = 0; i < buffers; i++) {
            mFrames.emplace_back(mDevice->createHeap<VulkanFrameSync>());
            mRenderFinishedSemaphores.emplace_back(mDevice->createHeap<Semaphore>());
        }
    }

    VulkanFrameSync& Surface::getCurrentFrame() const {
        return *mFrames[mCurrentFrame];
    }
}
