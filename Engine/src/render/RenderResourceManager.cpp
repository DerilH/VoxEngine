#include "VoxEngine/render/RenderResourceManager.h"

Vox::Render::RenderResourceManager::RenderResourceManager(const Ref<RenderBackend> backend)
    : mBackend(backend), mUniformAllocator(backend) {
}

Vox::Ref<Vox::Render::UniformBuffer> Vox::Render::RenderResourceManager::allocateUniformBuffer(size_t size) {
    return mUniformAllocator.allocate(size);
}

Vox::Render::PooledUniformBuffer Vox::Render::RenderResourceManager::allocatePooledUniformBuffer(size_t size) {
    return mUniformAllocator.allocatePooled(size);
}

void Vox::Render::RenderResourceManager::freeUniformBuffer(Ref<UniformBuffer> buffer) {
    mUniformAllocator.free(buffer);
}

void Vox::Render::RenderResourceManager::clearFreeBuffers() {
    mUniformAllocator.clearFreeBuffers();
}

Vox::Ref<Vox::Render::PipelineState> Vox::Render::RenderResourceManager::getPipeline(ConstRef<PipelineStateDesc> desc) {
    decltype(auto) it = mPipelineStateByHash[*desc];
    if (it == nullptr) {
        it = createPSO(desc);
    }
    return it;
}

Vox::Ref<Vox::Render::RenderMesh> Vox::Render::RenderResourceManager::getMesh(ConstRef<Resources::MeshAsset> asset) {
    decltype(auto) mesh = mLoadedMeshes[asset->getPath()];
    if (mesh == nullptr) {
        VertexBufferRef vBuff = mBackend->createVertexBuffer(data(asset->getInterleaved()), sizeof(char8_t) * asset->getInterleaved().size(), BufferUsage::VERTEX);
        IndexBufferRef iBuff = mBackend->createIndexBuffer(data(asset->getIndices()), sizeof(uint32_t) * asset->getIndices().size(), IndexType::UINT32);
        mesh = new RenderMesh(vBuff, iBuff);
    }
    return mesh;
}
