#include "VoxEngine/render/RenderResourceManager.h"

#include <VoxEngine/render/vulkan/VulkanDescriptorSet.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>
#include <VoxEngine/render/vulkan/buffers/VulkanUniformBuffer.h>

Vox::Render::RenderResourceManager::RenderResourceManager(const Ref<RenderBackend> backend) : mBackend(backend) {
}

Vox::Ref<Vox::Render::PipelineState> Vox::Render::RenderResourceManager::getPipeline(PipelineStateDesc desc) {
    decltype(auto) it = mPipelineStateByHash[desc];
    if (it == nullptr) {
        it = mBackend->createPSO(desc);
    }
    return it;
}

Vox::Ref<Vox::Render::RenderMesh> Vox::Render::RenderResourceManager::getMesh(ConstRef<Resources::MeshAsset> asset) {
    decltype(auto) mesh = mLoadedMeshes[asset->getPath()];
    if (mesh == nullptr) {
        VertexBufferRef vBuff = mBackend->createVertexBuffer(data(asset->getVertices()), sizeof(glm::vec3) * asset->getVertices().size(), BufferUsage::VERTEX);
        IndexBufferRef iBuff = mBackend->createIndexBuffer(data(asset->getIndices()), sizeof(uint32_t) * asset->getIndices().size(), IndexType::UINT32);
        mesh = new RenderMesh(vBuff, iBuff);

        auto ubo = mBackend->createUniformBuffer(sizeof(glm::mat4));

        mesh->setModelUbo(ubo);

        auto vkDevice = Vulkan::ResourceCast(mBackend->getDevice());
        auto layout = vkDevice->getModelDescriptorSetLayout();
        auto set = vkDevice->createDescriptorSet(layout);
        set->update(*vkDevice, 0, *static_cast<Vulkan::VulkanUniformBuffer *>(ubo));
        mesh->setDescriptorSet(set);
        static_cast<Vulkan::VulkanUniformBuffer *>(ubo)->setDescriptorSet(set);
    }
    return mesh;
}
