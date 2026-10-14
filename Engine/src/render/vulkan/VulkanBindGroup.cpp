//
// Created by deril on 10/14/26.
//
#include <VoxEngine/render/vulkan/VulkanBindGroup.h>
#include <VoxCore/Define.h>
VULKAN_NS
    VulkanBindGroup::VulkanBindGroup(Ref<VulkanDescriptorSet> set, uint8_t setId) : mSet(set), mSetId(setId) {
    }
NS_END
