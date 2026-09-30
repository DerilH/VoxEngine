#include <VoxCore/Assert.h>
#include <VoxCore/Define.h>
#include <VoxEngine/render/vulkan/VulkanDescriptorSet.h>
#include <VoxEngine/render/vulkan/VulkanTypes.h>
#include <VoxEngine/render/vulkan/VulkanDescriptorPool.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/buffers/VulkanUniformBuffer.h>
//
// Created by deril on 3/22/26.
//
VULKAN_NS
    VulkanDescriptorSet VulkanDescriptorSet::Create(const VulkanDevice &device, VulkanDescriptorPoolRef pool, VkDescriptorSetLayout layout) {
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = *pool;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &layout;

        VkDescriptorSet set;
        VK_CHECK(vkAllocateDescriptorSets(device, &allocateInfo, &set), "Cannot allocate descriptor set");
        return {set, layout};
    }

    VkDescriptorSetLayout VulkanDescriptorSet::getLayouts() const {
        return mLayout;
    }

    void VulkanDescriptorSet::update(const VulkanDevice& device, uint32_t binding, const VulkanUniformBuffer& buffer) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer.getHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = mHandle;
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
NS_END
