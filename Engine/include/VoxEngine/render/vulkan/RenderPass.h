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

        static int sIndex;
        const RenderPassType mType;
        const int mId;

        explicit RenderPass(VkRenderPass renderPass, RenderPassType type);

        static RenderPass Create(const LogicalDevice &device, VkFormat format, RenderPassType type);

    public:

        int getId() const;

        RenderPassType getType() const;

        bool operator==(const RenderPass &other) const;
    };
NS_END

template<>
struct std::hash<Vox::Render::Vulkan::RenderPass> {
    std::size_t operator()(const Vox::Render::Vulkan::RenderPass &k) const noexcept {
        return std::hash<uint32_t>{}(k.getId());
    }
};