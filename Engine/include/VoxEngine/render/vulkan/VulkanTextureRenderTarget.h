//
// Created by deril on 2/27/26.
//

#pragma once


#include "VoxEngine/render/Texture.h"
#include "VulkanRenderTarget.h"
#include "VulkanTexture.h"
#include "VoxEngine/render/Enums.h"

VULKAN_NS
class LogicalDevice;
class VulkanTextureRenderTarget : public VulkanRenderTarget, public VulkanTexture {
    friend class LogicalDevice;
protected:
    explicit VulkanTextureRenderTarget(const LogicalDevice* device, TextureHandle handle, VkImage image, VkFormat format, VkImageView view, VmaAllocation allocation, VkExtent2D extent);

public:
    static VulkanTextureRenderTarget Create(const LogicalDevice& device, TextureHandle handle, VkExtent2D extent, VkFormat format, VkImageUsageFlags usage);


    bool begin() override;

    void present() override;

    VkImageView getImageView() const override;

    VkImage getImage() const override;

    Format getFormat() const override;
};
NS_END
