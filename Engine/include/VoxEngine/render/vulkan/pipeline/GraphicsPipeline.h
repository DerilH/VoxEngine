//
// Created by deril on 2/14/26.
//

#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "VoxCore/containers/ArrayView.h"
#include "VoxEngine/render/state/RasterizerState.h"
#include "VoxEngine/render/vulkan/VulkanCommandBuffer.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "VoxCore/containers/Containers.h"
#include "BlendState.h"

VULKAN_NS
    class VulkanDevice;
    class PipelineBuilder;
    class RenderPass;

    class GraphicsPipeline : public VulkanObject<VkPipeline> {
        friend class VulkanDevice;
        friend class PipelineBuilder;

        VkPipelineLayout mLayout;

        GraphicsPipeline(VkPipeline pipeline, VkPipelineLayout layout);

        static PipelineBuilder Builder(const VulkanDevice &device);

    public:

        void bind(const CommandBuffer &cmdBuffer, VkPipelineBindPoint bindPoint) const;

        VkPipelineLayout getLayout() const;
    };

    class PipelineBuilder {
        const VulkanDevice &mDevice;
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
        explicit PipelineBuilder(const VulkanDevice &mDevice);

        PipelineBuilder &shader(const ArrayView<uint32_t> &code, VkShaderStageFlagBits stageFlags, InternedString name) &;

        PipelineBuilder &vertexInput(const ArrayView<VkVertexInputAttributeDescription> &attributes, const ArrayView<VkVertexInputBindingDescription> bindings) &;

        PipelineBuilder &topology(VkPrimitiveTopology topology, VkBool32 enableRestart) &;

        PipelineBuilder &viewport(uint32_t viewportsCount, uint32_t scissorsCount) &;

        PipelineBuilder &rasterizer(const RasterizerState &state) &;

        PipelineBuilder &msaa(VkSampleCountFlagBits flags = VK_SAMPLE_COUNT_1_BIT, VkBool32 enableShading = VK_FALSE) &;

        PipelineBuilder &dynamic(ArrayView<VkDynamicState> states) &;

        PipelineBuilder &layout(ArrayView<VkDescriptorSetLayout> sets, ArrayView<VkPushConstantRange> constants) &;

        PipelineBuilder &rendering(const ArrayView<VkFormat> &formats) &;

        BUILDER_ENTRY(PipelineBuilder, blend, BlendState, mBlend)

        GraphicsPipeline build() &;

    };
NS_END
