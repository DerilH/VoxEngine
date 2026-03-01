//
// Created by deril on 2/25/26.
//

#pragma once

#include "VoxEngine/render/Texture.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "VoxEngine/render/graph/TextureHandle.h"
#include "VoxEngine/render/Enums.h"

VULKAN_NS
class LogicalDevice;
class VulkanTexture : public Texture, public VulkanAllocated<VkImage> {
    friend class LogicalDevice;
    VkImageView mView;

protected:
    explicit VulkanTexture(VkImage image, TextureHandle handle, VkFormat format, VkImageView view, VmaAllocation allocation);
    static VulkanTexture Create(const LogicalDevice &device, TextureHandle handle, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage);
public:
    const Format format;
    VkImageView getView() const;
};
NS_END
