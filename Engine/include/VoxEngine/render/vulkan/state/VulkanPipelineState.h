//
// Created by deril on 3/13/26.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include "VoxCore/Define.h"
#include "VoxEngine/render/state/PipelineState.h"
#include "VoxEngine/render/CommandBuffer.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "VoxEngine/render/vulkan/VulkanDevice.h"

VULKAN_NS
    class VulkanDevice;
    class VulkanPipelineState : public PipelineState, public VulkanObject<VkPipeline> {
        friend class VulkanDevice;
        friend class VulkanCommandBuffer;
        VkPipelineLayout mLayout;
        Vector<VkDescriptorSetLayout> mDescriptorSetLayouts;

        VulkanPipelineState(const PipelineStateDesc& desc, VkPipeline handle, VkPipelineLayout layout, Vector<DescriptorSetInfo> descInfo = {}, Vector<VkDescriptorSetLayout> layouts = {});

        static VulkanPipelineState Create(const VulkanDevice &device, PipelineStateDesc desc, const VkGraphicsPipelineCreateInfo& info, Vector<DescriptorSetInfo> descInfo = {}, Vector<VkDescriptorSetLayout> layouts = {});
    public:
        void bind(CommandBufferRef cmdBuffer) override {
            cmdBuffer->bindPipelineState(this);
        }

        VkPipelineLayout getPipelineLayout() const {
            return mLayout;
        }

        VkDescriptorSetLayout getDescriptorSetLayout(uint32_t set) const {
            if (set < mDescriptorSetLayouts.size()) {
                return mDescriptorSetLayouts[set];
            }
            return VK_NULL_HANDLE;
        }

        const Vector<VkDescriptorSetLayout>& getDescriptorSetLayouts() const {
            return mDescriptorSetLayouts;
        }
    };
NS_END
