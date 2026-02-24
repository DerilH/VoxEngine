//
// Created by deril on 2/13/26.
//

#pragma once

#include <cstddef>
#include <vulkan/vulkan_core.h>
#include <functional>
#include <VoxEngine/render/RenderPassType.h>

#include "VoxEngine/render/vulkan/pipeline/GraphicsPipeline.h"

VULKAN_NS
    class LogicalDevice;

    class RenderPass : public VulkanObject<VkRenderPass>{
        friend class LogicalDevice;

        const RenderPassType mType;
        explicit RenderPass(VkRenderPass renderPass, RenderPassType type);

        static RenderPass Create(const LogicalDevice &device, VkFormat format, RenderPassType type, bool dynamic = true);

    public:

        RenderPassType getType() const;
    };
NS_END