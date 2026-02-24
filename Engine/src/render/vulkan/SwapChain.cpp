//
// Created by deril on 2/13/26.
//

#include <VoxEngine/render/vulkan/SwapChain.h>
#include <VoxEngine/render/vulkan/RenderPass.h>
#include <VoxEngine/render/vulkan/Surface.h>
#include <VoxEngine/render/vulkan/LogicalDevice.h>

namespace Vox::Render::Vulkan {
    VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto &availablePresentMode: availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const int width, const int height) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    SwapChain::SwapChain(const LogicalDevice &device, const VkSwapchainKHR mHandle,
                         std::vector<VkImage> images,
                         std::vector<VkImageView> imageViews,
                         const VkExtent2D extent) : VulkanObject(mHandle), mImages(std::move(images)),
                                                                       mImageViews(std::move(imageViews)), mDevice(device), mExtent(extent) {
    }

    VkImageView createImageView(const LogicalDevice &device, const VkImage image, const VkFormat format) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkImageView view;
        VK_CHECK(vkCreateImageView(device.getHandle(), &createInfo, nullptr, &view), "failed to create image views!");
        return view;
    }

    std::vector<VkFramebuffer> SwapChain::createFramebuffers(const LogicalDevice &device, const RenderPass renderPass) const {
        VOX_NO_IMPL("createFramebuffers");
        std::vector<VkFramebuffer> framebuffers(mImageViews.size());
        for (size_t i = 0; i < mImageViews.size(); i++) {
            const VkImageView attachments[] = {mImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass.getHandle();
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = mExtent.width;
            framebufferInfo.height = mExtent.height;
            framebufferInfo.layers = 1;

            int index;
            VK_CHECK(vkCreateFramebuffer(device.getHandle(), &framebufferInfo, nullptr, &framebuffers[i]),"failed to create framebuffer!");
        }
        return framebuffers;
    }

    SwapChain *SwapChain::Create(const Surface &surface, VkSwapchainKHR old) {
        VOX_CHECK(surface.getCurrentDevice() != nullptr, "Cant create swapchain, set surface device first");
        vkDeviceWaitIdle(surface.getCurrentDevice()->getHandle());

        SwapChainSupportDetails swapChainSupport = surface.querySwapChainSupport();
        VkPresentModeKHR presentMode = SwapChain::chooseSwapPresentMode(swapChainSupport.presentModes());
        VkExtent2D extent = SwapChain::chooseSwapExtent(swapChainSupport.capabilities(), surface.mWindow.getWidth(),surface.mWindow.getHeight());

        uint32_t imageCount = swapChainSupport.capabilities().minImageCount + 1;
        if (swapChainSupport.capabilities().maxImageCount > 0 && imageCount > swapChainSupport.capabilities().maxImageCount) {
            imageCount = swapChainSupport.capabilities().maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface.mHandle;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surface.getCurrentFormat().format;
        createInfo.imageColorSpace = surface.getCurrentFormat().colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const QueueFamilyRepository families = surface.mCurrentDevice->getPhysicalDevice().getQueueFamilies();
        const uint32_t queueFamilyIndices[] = {
            static_cast<uint32_t>(families[GRAPHICS_QUEUE].index()),
            static_cast<uint32_t>(surface.mPresentQueue->getFamily().index())
        };
        if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities().currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = old;

        VkSwapchainKHR handle = VK_NULL_HANDLE;
        VK_CHECK(vkCreateSwapchainKHR(surface.mCurrentDevice->getHandle(), &createInfo, nullptr, &handle), "failed to create swap chain!");

        vkGetSwapchainImagesKHR(surface.mCurrentDevice->getHandle(), handle, &imageCount, nullptr);
        std::vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(surface.mCurrentDevice->getHandle(), handle, &imageCount, images.data());

        std::vector<VkImageView> imageViews(imageCount);
        for (auto const &[index, image]: std::views::enumerate(images)) {
            VkImageView view = createImageView(*surface.mCurrentDevice, image, surface.getCurrentFormat().format);
            imageViews[index] = view;
        }

        return new SwapChain(*surface.mCurrentDevice, handle, std::move(images), std::move(imageViews), extent);
    }

    void SwapChain::addRenderPass(const RenderPass& renderPass) {
        VOX_NO_IMPL("addRenderPass");
        mFramebuffers.emplace(renderPass.getType(), createFramebuffers(mDevice, renderPass));
    }

    VkResult SwapChain::acquireNextImage(const Semaphore &semaphore, uint32_t* imageIndex) const {
        return vkAcquireNextImageKHR(mDevice.getHandle(), this->getHandle(), UINT64_MAX, semaphore.getHandle(), VK_NULL_HANDLE, imageIndex);
    }

    SwapChain::~SwapChain() {
        for (const auto& pass : mFramebuffers | std::views::values) {
            for (const auto& framebuffer : pass) {
                vkDestroyFramebuffer(mDevice.getHandle(), framebuffer, nullptr);
            }
        }
        for (const auto &view: mImageViews) {
            vkDestroyImageView(mDevice.getHandle(), view, nullptr);
        }
        vkDestroySwapchainKHR(mDevice.getHandle(), mHandle, nullptr);
    }

    VkFramebuffer SwapChain::operator[](const RenderPassType &renderPassType, const int index) const {
        return mFramebuffers.at(renderPassType).at(index);
    }

    VkImageView SwapChain::operator[](const int index) const {
        return mImageViews.at(index);
    }

    VkExtent2D SwapChain::getExtent() const {
        return mExtent;
    }

    int SwapChain::getImageCount() const {
        return mImages.size();
    }

    bool SwapChain::needsRebuild() const {
        return mNeedsRebuild;
    }

    void SwapChain::markForRebuild() {
        mNeedsRebuild = true;
    }
}
