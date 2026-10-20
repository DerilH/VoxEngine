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

Vox::Ref<Vox::Render::RenderMesh> Vox::Render::RenderResourceManager::getMesh(ConstRef<Resources::MeshResource> meshRes) {
    if (!meshRes) return nullptr;
    decltype(auto) mesh = mLoadedMeshes[meshRes->getName()];
    if (mesh == nullptr) {
        VertexBufferRef vBuff = mBackend->createVertexBuffer(data(meshRes->getInterleaved()), sizeof(char8_t) * meshRes->getInterleaved().size(), BufferUsage::VERTEX);
        IndexBufferRef iBuff = mBackend->createIndexBuffer(data(meshRes->getIndices()), sizeof(uint32_t) * meshRes->getIndices().size(), IndexType::UINT32);
        mesh = new RenderMesh(vBuff, iBuff);
    }
    return mesh;
}

Vox::Ref<Vox::Render::Texture> Vox::Render::RenderResourceManager::getTexture(Resources::TextureHandle handle, Ref<Resources::ResourceManager> resourceManager) {
    if (!handle.isValid() || !resourceManager) {
        return mDefaultTexture;
    }

    auto key = handle.toString();
    auto it = mLoadedTextures.find(key);
    if (it != mLoadedTextures.end() && it->second) {
        return it->second;
    }

    auto texRes = resourceManager->get(handle);
    if (texRes.has_value()) {
        auto pixels = (*texRes)->getData();
        auto size = (*texRes)->getSize();
        auto extent = (*texRes)->getExtent();
        auto format = (*texRes)->getFormat();
        auto gpuTex = mBackend->createTexture(format, extent, (void*)pixels, size);
        mLoadedTextures[key] = gpuTex;
        return gpuTex;
    }

    return mDefaultTexture;
}

Vox::Ref<Vox::Render::BindGroup> Vox::Render::RenderResourceManager::getMaterialTextureGroup(
    ConstRef<Resources::MaterialResource> material,
    Ref<PipelineState> pso,
    Ref<Resources::ResourceManager> resourceManager
) {
    if (!material) {
        return mSamplerGroup;
    }

    auto gpuTex = getTexture(material->getTexture(), resourceManager);
    if (!gpuTex) {
        gpuTex = mDefaultTexture;
    }

    auto it = mMaterialTextureBindings.find(material);
    if (it != mMaterialTextureBindings.end()) {
        if (it->second.bindGroup) {
            if (it->second.texture != gpuTex) {
                if (gpuTex) {
                    updateBindGroup(it->second.bindGroup, 0, gpuTex);
                }
                it->second.texture = gpuTex;
            }
            return it->second.bindGroup;
        }
    }

    if (!pso || !gpuTex) {
        return mSamplerGroup;
    }

    auto bindGroup = createTextureBindGroup(pso, 1, 0, gpuTex);
    if (!bindGroup) {
        return mSamplerGroup;
    }

    MaterialTextureBinding binding{};
    binding.texture = gpuTex;
    binding.bindGroup = bindGroup;
    mMaterialTextureBindings[material] = binding;
    return bindGroup;
}
