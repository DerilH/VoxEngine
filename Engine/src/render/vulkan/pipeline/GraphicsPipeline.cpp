////
//// Created by deril on 2/14/26.
////
//
//#include <stdexcept>
//#include <vector>
//#include "VoxEngine/render/vulkan/pipeline/GraphicsPipeline.h"
//#include "VoxEngine/render/vulkan/LogicalDevice.h"
//#include "VoxEngine/render/vulkan/Vertex.h"
//#include <vulkan/vulkan_core.h>
//#include "VoxEngine/render/vulkan/VulkanEnums.h"
//#include "VoxCore/containers/ArrayView.h"
//
//namespace Vox::Render::Vulkan {
//    GraphicsPipeline::GraphicsPipeline(VkPipeline pipeline, VkPipelineLayout layout) : VulkanObject(pipeline), mLayout(layout) {
//    }
//
////    VkShaderModule createShaderModule(const LogicalDevice &device, const Vox::ArrayView<uint32_t> &code) {
////        VkShaderModuleCreateInfo createInfo{};
////        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
////        createInfo.codeSize = code.sizeInBytes();
////        createInfo.pCode = code.pData;
////
////        VkShaderModule shaderModule;
////        VK_CHECK(vkCreateShaderModule(device.getHandle(), &createInfo, nullptr, &shaderModule), "failed to create shader module!");
////        return shaderModule;
////    }
//
//    VkPipelineLayout GraphicsPipeline::getLayout() const {
//        return mLayout;
//    }
//
//    void GraphicsPipeline::bind(const CommandBuffer &cmdBuffer, VkPipelineBindPoint bindPoint) const {
////        vkCmdBindPipeline(cmdBuffer, bindPoint, mHandle);
//    }
//
//    PipelineBuilder GraphicsPipeline::Builder(const LogicalDevice &device) {
//        return PipelineBuilder(device);
//    }
//
//    PipelineBuilder &PipelineBuilder::shader(const ArrayView<uint32_t> &code, VkShaderStageFlagBits stageFlags, InternedString name) &{
////        VkShaderModule module = createShaderModule(mDevice, code);
////        VkPipelineShaderStageCreateInfo info{};
////        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
////        info.pName = name.c_str();
////        info.module = module;
////        info.stage = stageFlags;
////
////        mShaderStages.emplace_back(info);
////        return *this;
////    }
//
//    PipelineBuilder::PipelineBuilder(const LogicalDevice &mDevice) : mDevice(mDevice) {}
//
//    PipelineBuilder &PipelineBuilder::vertexInput(const ArrayView<VkVertexInputAttributeDescription> &attributes, const ArrayView<VkVertexInputBindingDescription> bindings) &{
//        mVertexInputInfo = VkPipelineVertexInputStateCreateInfo();
//        mVertexInputInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//
//        mVertexInputInfo->vertexBindingDescriptionCount = bindings.size();
//        mVertexInputInfo->pVertexBindingDescriptions = bindings.pData;
//
//        mVertexInputInfo->vertexAttributeDescriptionCount = attributes.size();
//        mVertexInputInfo->pVertexAttributeDescriptions = attributes.pData;
//        return *this;
//    }
//
////    PipelineBuilder &PipelineBuilder::topology(VkPrimitiveTopology topology, VkBool32 enableRestart) &{
////        mInputAssembly = VkPipelineInputAssemblyStateCreateInfo();
////        mInputAssembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
////        mInputAssembly->topology = topology;
////        mInputAssembly->primitiveRestartEnable = enableRestart;
////
////        return *this;
////    }
//
//    PipelineBuilder &PipelineBuilder::viewport(uint32_t viewportsCount, uint32_t scissorsCount) &{
//        mViewportInfo = VkPipelineViewportStateCreateInfo();
//        mViewportInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//        mViewportInfo->viewportCount = viewportsCount;
//        mViewportInfo->scissorCount = scissorsCount;
//        return *this;
//    }
//
////    PipelineBuilder &PipelineBuilder::rasterizer(const RasterizerState &state) &{
////        mRasterizer = VkPipelineRasterizationStateCreateInfo();
////        mRasterizer->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
////        mRasterizer->depthClampEnable = static_cast<VkBool32>(state.depthClampEnable());
////        mRasterizer->rasterizerDiscardEnable = static_cast<VkBool32>(state.rasterizerDiscardEnable());
////        mRasterizer->polygonMode = toVk(state.polygonMode());
////        mRasterizer->lineWidth = state.lineWidth();
////        mRasterizer->cullMode = toVk(state.cullMode());
////        mRasterizer->frontFace = toVk(state.frontFace());
////        mRasterizer->depthBiasEnable = static_cast<VkBool32>(state.depthBiasEnable());
////        return *this;
////    }
//
////    PipelineBuilder &PipelineBuilder::msaa(VkSampleCountFlagBits flags, VkBool32 enableShading) &{
////        mMSAA = VkPipelineMultisampleStateCreateInfo();
////        mMSAA->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
////        mMSAA->sampleShadingEnable = enableShading;
////        mMSAA->rasterizationSamples = flags;
////        return *this;
////    }
//
//    PipelineBuilder &PipelineBuilder::dynamic(const ArrayView<VkDynamicState> states) &{
//        mDynamicState = VkPipelineDynamicStateCreateInfo();
//        mDynamicState->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//        mDynamicState->dynamicStateCount = states.size();
//        mDynamicState->pDynamicStates = states.pData;
//        return *this;
//    }
//
////    PipelineBuilder &PipelineBuilder::layout(const ArrayView<VkDescriptorSetLayout> sets, const ArrayView<VkPushConstantRange> constants) &{
////        mLayout = VkPipelineLayoutCreateInfo();
////        mLayout->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
////        mLayout->setLayoutCount = sets.size();
////        mLayout->pSetLayouts = sets.pData;
////
////        mLayout->pushConstantRangeCount = constants.size();
////        mLayout->pPushConstantRanges = constants.pData;
////
//////        VkPipelineLayout layout;
//////        VK_CHECK(vkCreatePipelineLayout(mDevice, &mLayout, nullptr, &layout),"failed to create pipeline layout!");
////        return *this;
////    }
//
////    PipelineBuilder &PipelineBuilder::rendering(const ArrayView<VkFormat> &formats) &{
////        mRendering = VkPipelineRenderingCreateInfo();
////        mRendering->sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
////        mRendering->colorAttachmentCount = formats.size();
////        mRendering->pColorAttachmentFormats = formats.pData;
////        mRendering->depthAttachmentFormat = VK_FORMAT_UNDEFINED;
////        mRendering->stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
////        return *this;
////    }
//
//    GraphicsPipeline PipelineBuilder::build() &{
//        VOX_ASSERT(!mShaderStages.empty(), "Builder value not present");
//        VOX_ASSERT(mVertexInputInfo.has_value(), "Builder value not present");
//        VOX_ASSERT(mInputAssembly.has_value(), "Builder value not present");
//        VOX_ASSERT(mViewportInfo.has_value(), "Builder value not present");
//        VOX_ASSERT(mRasterizer.has_value(), "Builder value not present");
//        VOX_ASSERT(mMSAA.has_value(), "Builder value not present");
//        VOX_ASSERT(mDynamicState.has_value(), "Builder value not present");
//        VOX_ASSERT(mLayout.has_value(), "Builder value not present");
//        VOX_ASSERT(mRendering.has_value(), "Builder value not present");
//
//        VkGraphicsPipelineCreateInfo pipelineInfo{};
//        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//        pipelineInfo.stageCount = mShaderStages.size();
//        pipelineInfo.pStages = mShaderStages.data();
//        pipelineInfo.pVertexInputState = &mVertexInputInfo.value();
//        pipelineInfo.pInputAssemblyState = &mInputAssembly.value();
//        pipelineInfo.pViewportState = &mViewportInfo.value();
//        pipelineInfo.pRasterizationState = &mRasterizer.value();
//        pipelineInfo.pMultisampleState = &mMSAA.value();
//        pipelineInfo.pColorBlendState = &mBlend.value().getState();
//        pipelineInfo.pDynamicState = &mDynamicState.value();
//
//        VkPipelineLayout l;
//        vkCreatePipelineLayout(mDevice,&mLayout.value(),nullptr,&l);
//
//        pipelineInfo.layout = l;
//        pipelineInfo.subpass = 0;
//        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//        pipelineInfo.pNext = &mRendering.value();
//
//        VkPipeline pipeline;
//        VK_CHECK(vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline), "failed to create graphics pipeline!")
//
//        for(const auto& shader : mShaderStages) {
//            vkDestroyShaderModule(mDevice, shader.module, nullptr);
//        }
//
//        return {pipeline, l};
//    }
//
//}
