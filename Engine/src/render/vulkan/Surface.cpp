//
// Created by deril on 2/13/26.
//


#include <VoxEngine/render/vulkan/LogicalDevice.h>
#include <VoxEngine/render/vulkan/Surface.h>
#include <VoxEngine/render/vulkan/FrameSync.h>
#include <VoxEngine/render/vulkan/SwapChain.h>

namespace Vox::Render::Vulkan {
    SwapChainSupportDetails Surface::querySwapChainSupport() const {
        VkSurfaceCapabilitiesKHR capabilities;

        VkPhysicalDevice device = mCurrentDevice->getPhysicalDevice().getHandle();
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
        for (const auto &family: mCurrentDevice->getPhysicalDevice().getQueueFamilies().getUniqueFamilies()) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(mCurrentDevice->getPhysicalDevice().getHandle(), family.index(), mHandle, &presentSupport);

            if (presentSupport) {
                return family;
            }
        }
        return std::nullopt;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto &availableFormat: availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }


    void Surface::setDevice(LogicalDevice *device) {
        mCurrentDevice = device;
        const std::optional<QueueFamily> family = findPresentFamily();
        VOX_CHECK(family.has_value(), "Device cant be used for present");

        mPresentQueue = &(device->getQueues().at(family->type()));
        mCurrentFormat = chooseSwapSurfaceFormat(querySwapChainSupport().formats());
        createSwapChain();
    }

    Surface::Surface(const Windowing::Window &window, VkSurfaceKHR handle) : mWindow(window), VulkanObject(handle) {
    }

    Surface *Surface::Create(VkInstance instance, const Windowing::Window &window) {
        VkSurfaceKHR s = nullptr;
        VK_CHECK(glfwCreateWindowSurface(instance, window.getHandle(), nullptr, &s),
                 "failed to create window surface!");
        return new Surface{window, s};
    }

    void Surface::update() {
        if (mCurrentSwapChain != nullptr && mCurrentSwapChain->needsRebuild()) {
            createSwapChain();
        }
    }

    void Surface::createSwapChain() {
        SwapChain *old = nullptr;
        if (mCurrentSwapChain != nullptr) {
            old = mCurrentSwapChain;
        }
        mCurrentSwapChain = SwapChain::Create(*this, old == nullptr ? nullptr : old->getHandle());
    }

    SwapChain &Surface::getSwapChain() const {
        return *mCurrentSwapChain;
    }

    VkSurfaceFormatKHR Surface::getCurrentFormat() const {
        return mCurrentFormat;
    }

    const LogicalDevice *Surface::getCurrentDevice() const {
        return mCurrentDevice;
    }

    const Queue *Surface::getPresentQueue() const {
        return mPresentQueue;
    }

    void Surface::presentFrame(const FrameSync &frame) const {
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

        vkQueuePresentKHR(mPresentQueue->getHandle(), &presentInfo);
    }
}
