//
// Created by deril on 2/14/26.
//

#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "VoxCore/containers/Buffer.h"
#include "VoxEngine/render/vulkan/CommandBuffer.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "VoxCore/containers/Containers.h"
#include "BlendState.h"

VULKAN_NS
    class LogicalDevice;
    class PipelineBuilder;
    class RenderPass;

    class GraphicsPipeline : public VulkanObject<VkPipeline> {
        friend class LogicalDevice;
        friend class PipelineBuilder;

        VkPipelineLayout mLayout;

        GraphicsPipeline(VkPipeline pipeline, VkPipelineLayout layout);

        static PipelineBuilder Builder(const LogicalDevice &device);

    public:

        void bind(const CommandBuffer &cmdBuffer, VkPipelineBindPoint bindPoint) const;

        VkPipelineLayout getLayout() const;
    };

    class PipelineBuilder {
        const LogicalDevice &mDevice;
        SmallVector<VkPipelineShaderStageCreateInfo> mShaderStages{};
        std::optional<VkPipelineVertexInputStateCreateInfo> mVertexInputInfo{};
        std::optional<VkPipelineInputAssemblyStateCreateInfo> mInputAssembly{};
        std::optional<VkPipelineViewportStateCreateInfo> mViewportInfo{};
        std::optional<VkPipelineRasterizationStateCreateInfo> mRasterizer{};
        std::optional<VkPipelineMultisampleStateCreateInfo> mMSAA{};
        std::optional<VkPipelineDynamicStateCreateInfo> mDynamicState{};
        std::optional<VkPipelineLayoutCreateInfo> mLayout{};
        std::optional<VkPipelineRenderingCreateInfo> mRendering{};
        std::optional<BlendState> mBlend{};

        bool built = false;

    public:
        explicit PipelineBuilder(const LogicalDevice &mDevice);

        PipelineBuilder &shader(const Buffer<uint32_t> &code, VkShaderStageFlagBits stageFlags, InternedString name) &;

        PipelineBuilder &vertexInput(const Buffer<VkVertexInputAttributeDescription> &attributes, const Buffer<VkVertexInputBindingDescription> bindings) &;

        PipelineBuilder &topology(VkPrimitiveTopology topology, VkBool32 enableRestart) &;

        PipelineBuilder &viewport(uint32_t viewportsCount, uint32_t scissorsCount) &;

        PipelineBuilder &rasterizer(VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL, float lineWidth = 1, VkBool32 depthBias = VK_TRUE, VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT, VkFrontFace fronFace = VK_FRONT_FACE_CLOCKWISE) &;

        PipelineBuilder &msaa(VkSampleCountFlagBits flags = VK_SAMPLE_COUNT_1_BIT, VkBool32 enableShading = VK_FALSE) &;

        PipelineBuilder &dynamic(const Buffer<VkDynamicState> states) &;

        PipelineBuilder &layout(Buffer<VkDescriptorSetLayout> sets, Buffer<VkPushConstantRange> constants) &;

        PipelineBuilder &rendering(const Buffer<VkFormat> &formats) &;

        BUILDER_ENTRY(PipelineBuilder, blend, BlendState, mBlend)

        GraphicsPipeline build() &;

    };
NS_END
