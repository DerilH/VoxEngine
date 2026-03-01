//
// Created by deril on 2/27/26.
//

#include "VoxEngine/render/vulkan/VulkanTextureRenderTarget.h"
#include "VoxEngine/render/vulkan/LogicalDevice.h"
#include "VoxCore/math/Extent.h"

VULKAN_NS

    bool VulkanTextureRenderTarget::begin() {
        return true;
    }

    void VulkanTextureRenderTarget::present() {
    }

    VkImageView VulkanTextureRenderTarget::getImageView() const {
        return getView();
    }

    VkImage VulkanTextureRenderTarget::getImage() const {
        return VulkanAllocated::getHandle();
    }

    Format VulkanTextureRenderTarget::getFormat() const {
        return format;
    }

    VulkanTextureRenderTarget::VulkanTextureRenderTarget(const LogicalDevice* device, TextureHandle handle, VkImage image, VkFormat format, VkImageView view, VmaAllocation allocation, VkExtent2D extent) : VulkanRenderTarget(device, extent), VulkanTexture(image, handle, format, view, allocation) {
    }

    VulkanTextureRenderTarget VulkanTextureRenderTarget::Create(const LogicalDevice& device, TextureHandle handle, VkExtent2D extent, VkFormat format, VkImageUsageFlags usage) {
        auto allocator = device.getAllocator();
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent = {extent.width, extent.height, 1};
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        allocInfo.flags = 0;

        VmaAllocation allocation;
        VkImage image;
        VK_CHECK(vmaCreateImage(allocator, &imageInfo, &allocInfo, &image, &allocation, nullptr), "Cannot create VkImage");
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
        VK_CHECK(vkCreateImageView(device, &createInfo, nullptr, &view), "Cannot create VkImageView");
        return VulkanTextureRenderTarget(&device, handle, image, format, view, allocation, VkExtent2D(extent.width, extent.height));
    }
NS_END
