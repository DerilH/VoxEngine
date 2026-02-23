//
// Created by deril on 2/13/26.
//

#pragma once
#include <VoxEngine/render/windowing/Window.h>
#include <vulkan/vulkan_core.h>

namespace Vox::Render::Vulkan {
    class LogicalDevice;
    class QueueFamily;
    class Queue;
    class FrameSync;
    class SwapChain;

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities() const {
            return mCapabilities;
        }

        std::vector<VkSurfaceFormatKHR> formats() const {
            return mFormats;
        }

        std::vector<VkPresentModeKHR> presentModes() const {
            return mPresentModes;
        }

    private:
        VkSurfaceCapabilitiesKHR mCapabilities{};
        std::vector<VkSurfaceFormatKHR> mFormats;
        std::vector<VkPresentModeKHR> mPresentModes;

        SwapChainSupportDetails() = default;

        SwapChainSupportDetails(const VkSurfaceCapabilitiesKHR & capabilities, const std::vector<VkSurfaceFormatKHR> & formats, const std::vector<VkPresentModeKHR> & presentModes);
        friend class Surface;
    };

    class Surface {

        friend class LogicalDevice;
        friend class SwapChain;

        Windowing::Window mWindow;
        VkSurfaceKHR mHandle = VK_NULL_HANDLE;
        LogicalDevice* mCurrentDevice = nullptr;
        const Queue* mPresentQueue = nullptr;
        SwapChain* mCurrentSwapChain = nullptr;
        VkSurfaceFormatKHR mCurrentFormat;

        SwapChainSupportDetails querySwapChainSupport() const;
        std::optional<QueueFamily> findPresentFamily() const;

        explicit Surface(const Windowing::Window &window, VkSurfaceKHR handle);
        void createSwapChain();
    public:
        Surface() = delete;

        void setDevice(LogicalDevice* device);
        static Surface *Create(VkInstance instance, const Windowing::Window &window);

        void update();

        VkSurfaceKHR getHandle() const;
        SwapChain& getSwapChain() const;
        VkSurfaceFormatKHR getCurrentFormat() const;

        const LogicalDevice* getCurrentDevice() const;
        const Queue *getPresentQueue() const;
        void presentFrame(const FrameSync &frame) const;
    };
}
