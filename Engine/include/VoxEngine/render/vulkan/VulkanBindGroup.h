#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxEngine/render/BindGroup.h>

#include "VulkanDescriptorSet.h"

VULKAN_NS
    class VulkanBindGroup : public BindGroup {
        Ref<VulkanDescriptorSet> mSet;
        uint8_t mSetId;

        void t() override {
        }

    public:
        explicit VulkanBindGroup(Ref<VulkanDescriptorSet> set, uint8_t setId);

        uint8_t getSetId() const {
            return mSetId;
        }

        Ref<VulkanDescriptorSet> getSet() const {
            return mSet;
        }
    };

NS_END
