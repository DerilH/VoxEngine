//
// Created by deril on 2/13/26.
//

#pragma once
#include <vulkan/vulkan_core.h>

#include "RenderPass.h"
#include "Semaphore.h"

namespace Vox::Render::Vulkan {
    class LogicalDevice;
    class Surface;

    class SwapChain {
        friend class Surface;
        friend class LogicalDevice;

        VkSwapchainKHR mHandle;
        std::vector<VkImage> mImages;
        std::vector<VkImageView> mImageViews;
        const LogicalDevice& mDevice;
        VkExtent2D mExtent;
        std::unordered_map<RenderPassType, std::vector<VkFramebuffer>> mFramebuffers;
        bool mNeedsRebuild = false;

        SwapChain(const LogicalDevice &device, VkSwapchainKHR mHandle, std::vector<VkImage> images, std::vector<VkImageView> imageViews, VkExtent2D extent);

        std::vector<VkFramebuffer> createFramebuffers(const LogicalDevice &device, RenderPass renderPass) const;
        static SwapChain* Create(const Surface &surface, VkSwapchainKHR old = nullptr);
    public:
        void addRenderPass(const RenderPass &renderPass);

        VkResult acquireNextImage(const Semaphore &semaphore, uint32_t *imageIndex) const;

        SwapChain() = delete;
        ~SwapChain();

        VkFramebuffer operator[](const RenderPassType &renderPassType, int index) const;
        VkExtent2D getExtent() const;
        VkSwapchainKHR getHandle() const;
        int getImageCount() const;
        bool needsRebuild() const;
        void markForRebuild();

        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const int width, const int height);
    };
}
