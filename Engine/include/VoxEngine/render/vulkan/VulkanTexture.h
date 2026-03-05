//
// Created by deril on 2/25/26.
//

#pragma once

#include "VoxEngine/render/Texture.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "VoxEngine/render/Enums.h"

VULKAN_NS
class VulkanDevice;
class VulkanTexture : public Texture, public VulkanAllocated<VkImage> {
    friend class VulkanDevice;
    VkImageView mView;

    explicit VulkanTexture(Format format, Extent extent, VkImage image, VkImageView view, VmaAllocation allocation);
    static VulkanTexture Create(Format format, Extent extent, const VulkanDevice& device , VkImageUsageFlags usage);
public:
    VkImageView getView() const;
};
NS_END
