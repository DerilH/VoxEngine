#include "VoxEngine/render/RenderResourceManager.h"
#include <../../../Editor/src/assets/AssetManager.h>
#include <../../include/VoxEngine/resources/TextureAsset.h>

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

Vox::Ref<Vox::Render::Texture> Vox::Render::RenderResourceManager::getTexture(InternedString path, Ref<Resources::AssetManager> resourceManager) {
    if (path.empty()) {
        return mDefaultTexture;
    }

    auto it = mLoadedTextures.find(path);
    if (it != mLoadedTextures.end() && it->second) {
        return it->second;
    }

    if (!resourceManager) {
        return mDefaultTexture;
    }

    try {
        auto texAsset = resourceManager->get<Resources::TextureAsset>(path);
        if (texAsset.has_value()) {
            auto pixels = (*texAsset)->getPixels();
            auto extent = (*texAsset)->getExtent();
            auto gpuTex = mBackend->createTexture(Format::RGBA8, extent, pixels.pData, pixels.size());
            mLoadedTextures[path] = gpuTex;
            return gpuTex;
        }
    } catch (const std::exception& e) {
        LOG_WARN("Failed to load texture '{}': {}", path.c_str(), e.what());
    }

    return mDefaultTexture;
}

Vox::Ref<Vox::Render::BindGroup> Vox::Render::RenderResourceManager::getMaterialTextureGroup(
    ConstRef<Resources::MaterialAsset> material,
    Ref<PipelineState> pso,
    Ref<Resources::AssetManager> resourceManager
) {
    if (!material) {
        return mSamplerGroup;
    }

    auto gpuTex = getTexture(material->texture, resourceManager);
    if (!gpuTex) {
        gpuTex = mDefaultTexture;
    }

    auto it = mMaterialTextureBindings.find(material);
    if (it != mMaterialTextureBindings.end()) {
        if (it->second.bindGroup) {
            if (it->second.texturePath != material->texture || it->second.texture != gpuTex) {
                if (gpuTex) {
                    updateBindGroup(it->second.bindGroup, 0, gpuTex);
                }
                it->second.texturePath = material->texture;
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
    binding.texturePath = material->texture;
    binding.texture = gpuTex;
    binding.bindGroup = bindGroup;
    mMaterialTextureBindings[material] = binding;
    return bindGroup;
}
