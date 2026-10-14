#pragma once
#include <VoxCore/Define.h>
#include <VoxEngine/render/RenderResourceManager.h>

#include "VulkanDevice.h"
#include "buffers/VulkanUniformBuffer.h"

VULKAN_NS
    class VulkanRenderResourceManager : public RenderResourceManager {
        Ref<VulkanDescriptorPool> mGlobalDescPool = nullptr;
        Ref<VulkanDevice> mDevice;
        Ref<VulkanDescriptorSet> mCameraDescSet;
        Ref<VulkanDescriptorSet> mModelDescSet;

        Ref<PipelineState> createPSO(PipelineStateDesc &desc) override;
        Vector<VkDescriptorSetLayout> createDescriptorLayouts(const Vector<ShaderRef> &shaders);
        bool mCachePrepared = false;
    public:
        explicit VulkanRenderResourceManager(Ref<RenderBackend> backend, Ref<VulkanDevice> device);

        void prepareCache() override;
    };

NS_END
