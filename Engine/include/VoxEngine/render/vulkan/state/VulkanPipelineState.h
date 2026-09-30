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
        Vector<VkDescriptorSetLayout> mDescriptorLayouts;
        VkPipelineLayout mLayout;

        VulkanPipelineState(const PipelineStateDesc& desc, VkPipeline handle, VkPipelineLayout layout, Vector<VkDescriptorSetLayout>& descriptorLayouts);

        static VulkanPipelineState Create(const VulkanDevice& device, const PipelineStateDesc& desc);

    public:
        void bind(CommandBufferRef cmdBuffer) override {
            cmdBuffer->bindPipelineState(this);
        }
    };
NS_END
