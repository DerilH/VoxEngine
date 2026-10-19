//
// Created by deril on 3/13/26.
//

#include <VoxEngine/render/vulkan/VulkanUtil.h>
#include "VoxEngine/render/vulkan/state/VulkanPipelineState.h"

#include <VoxEngine/render/vulkan/VulkanDescriptorSet.h>

#include "VoxEngine/render/state/Shader.h"
#include "VoxEngine/render/vulkan/VulkanResourceCast.h"

VULKAN_NS
    VulkanPipelineState VulkanPipelineState::Create(const VulkanDevice &device, PipelineStateDesc desc, const VkGraphicsPipelineCreateInfo& info, Vector<DescriptorSetInfo> descInfo, Vector<VkDescriptorSetLayout> layouts) {
        VkPipeline pipeline{};
        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline), "Error creating graphics pipeline");
        return VulkanPipelineState(desc, pipeline, info.layout, std::move(descInfo), std::move(layouts));
    }

    VulkanPipelineState::VulkanPipelineState(const PipelineStateDesc &desc, VkPipeline handle, VkPipelineLayout layout, Vector<DescriptorSetInfo> descInfo, Vector<VkDescriptorSetLayout> layouts)
        : PipelineState(desc, std::move(descInfo)), VulkanObject(handle), mLayout(layout), mDescriptorSetLayouts(std::move(layouts)) {
    }

NS_END
