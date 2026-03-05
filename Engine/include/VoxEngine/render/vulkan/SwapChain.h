//
// Created by deril on 2/13/26.
//

#pragma once
#include <vulkan/vulkan_core.h>

#include "VoxEngine/render/passes/RenderPass.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "Semaphore.h"

VULKAN_NS
    class VulkanDevice;
    class Surface;

    class SwapChain : public VulkanObject<VkSwapchainKHR> {
        friend class Surface;
        friend class VulkanDevice;

        std::vector<VkImage> mImages;
        std::vector<VkImageView> mImageViews;
        const VulkanDevice& mDevice;
        VkExtent2D mExtent;
        bool mNeedsRebuild = false;

        SwapChain(const VulkanDevice &device, VkSwapchainKHR mHandle, std::vector<VkImage> images, std::vector<VkImageView> imageViews, VkExtent2D extent);

        static SwapChain* Create(const Surface &surface, VkSwapchainKHR old = nullptr);
    public:
        VkResult acquireNextImage(const Semaphore &semaphore, uint32_t *imageIndex) const;

        SwapChain() = delete;
        ~SwapChain();

        VkFramebuffer operator[](const RenderPassType &renderPassType, int index) const;
        VkImageView operator[](int index) const;
        VkImage getImage(int index) const;

        VkExtent2D getExtent() const;
        int getImageCount() const;
        bool needsRebuild() const;
        void markForRebuild();

        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const int width, const int height);
    };
NS_END