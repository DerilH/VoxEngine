//
// Created by deril on 2/14/26.
//

#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>
#include "VoxCore/containers/Buffer.h"
#include "CommandBuffer.h"
#include "VulkanObject.h"

namespace Vox::Render::Vulkan {
    class LogicalDevice;
    class RenderPass;

class GraphicsPipeline : public VulkanObject<VkPipeline>{
        friend class LogicalDevice;
        VkPipelineLayout mLayout;
        GraphicsPipeline(VkPipeline pipeline, VkPipelineLayout layout);

        static GraphicsPipeline Create(const LogicalDevice &device, const RenderPass & pass, const Vox::Buffer<uint32_t>* vertexShader, const Vox::Buffer<uint32_t>* fragmentShader);

    public:

        void bind(const CommandBuffer& cmdBuffer, VkPipelineBindPoint bindPoint) const;
        VkPipelineLayout getLayout() const;
    };
}
