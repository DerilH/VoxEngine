#pragma once
#include <VoxCore/Define.h>
#include <VoxEngine/render/RenderResourceManager.h>
#include <vulkan/vulkan_core.h>

#include "VulkanDevice.h"
#include "buffers/VulkanUniformBuffer.h"

namespace Vox::Render {
    struct DescriptorBindingInfo;
}

VULKAN_NS
    class VulkanDescriptorPool;
    class VulkanDescriptorSet;

    class VulkanRenderResourceManager : public RenderResourceManager {
        Vector<Ref<VulkanDescriptorPool>> mDescriptorPools;
        Ref<VulkanDevice> mDevice;
        Ref<VulkanDescriptorSet> mCameraDescSet;
        Ref<VulkanDescriptorSet> mModelDescSet;
        Ref<VulkanDescriptorSet> mSamplerDescSet;

        HashMap<uint64_t, VkDescriptorSetLayout> mDescriptorSetLayoutCache;
        VkDescriptorSetLayout mEmptyDescriptorSetLayout = VK_NULL_HANDLE;

        Ref<PipelineState> createPSO(ConstRef<PipelineStateDesc> desc) override;

        VkDescriptorSetLayout getOrCreateDescriptorSetLayout(const Vector<DescriptorBindingInfo>& bindings);
        Ref<VulkanDescriptorPool> getOrCreateDescriptorPool();

        bool mCachePrepared = false;

    public:
        explicit VulkanRenderResourceManager(Ref<RenderBackend> backend, Ref<VulkanDevice> device);
        ~VulkanRenderResourceManager() override;

        void updateSampler(Ref<Texture> texture) override;
        void prepareCache() override;

        Ref<BindGroup> createBindGroup(Ref<PipelineState> pso, uint32_t setId) override;
        Ref<BindGroup> createUniformBindGroup(Ref<PipelineState> pso, uint32_t setId, uint32_t binding, Ref<UniformBuffer> buffer) override;
        Ref<BindGroup> createTextureBindGroup(Ref<PipelineState> pso, uint32_t setId, uint32_t binding, Ref<Texture> texture) override;
        void updateBindGroup(Ref<BindGroup> bindGroup, uint32_t binding, Ref<UniformBuffer> buffer) override;
        void updateBindGroupDynamic(Ref<BindGroup> bindGroup, uint32_t binding, Ref<UniformBuffer> buffer, size_t range) override;
        void updateBindGroup(Ref<BindGroup> bindGroup, uint32_t binding, Ref<Texture> texture) override;
    };

NS_END
