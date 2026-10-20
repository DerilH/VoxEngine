//
// Created by deril on 2/20/26.
//

#include "VoxEngine/resources/ResourceManager.h"

RESOURCES_NS
    bool ResourceManager::isValidRaw(ResourceHandleRaw raw) const noexcept {
        if (!raw.isValid()) return false;
        switch (raw.type) {
            case ResourceType::MESH:
                return mMeshPool.isValid(ResourceHandle<MeshResource>(raw));
            case ResourceType::MODEL:
                return mModelPool.isValid(ResourceHandle<ModelResource>(raw));
            case ResourceType::MATERIAL:
                return mMaterialPool.isValid(ResourceHandle<MaterialResource>(raw));
            case ResourceType::SHADER:
                return mShaderPool.isValid(ResourceHandle<ShaderResource>(raw));
            case ResourceType::TEXTURE:
                return mTexturePool.isValid(ResourceHandle<TextureResource>(raw));
            default:
                return false;
        }
    }

    bool ResourceManager::removeRaw(ResourceHandleRaw raw) {
        if (!raw.isValid()) return false;
        switch (raw.type) {
            case ResourceType::MESH:
                return remove<MeshResource>(ResourceHandle<MeshResource>(raw));
            case ResourceType::MODEL:
                return remove<ModelResource>(ResourceHandle<ModelResource>(raw));
            case ResourceType::MATERIAL:
                return remove<MaterialResource>(ResourceHandle<MaterialResource>(raw));
            case ResourceType::SHADER:
                return remove<ShaderResource>(ResourceHandle<ShaderResource>(raw));
            case ResourceType::TEXTURE:
                return remove<TextureResource>(ResourceHandle<TextureResource>(raw));
            default:
                return false;
        }
    }

    void ResourceManager::clear() {
        mMeshPool.clear();
        mModelPool.clear();
        mMaterialPool.clear();
        mShaderPool.clear();
        mTexturePool.clear();
        mNamedResources.clear();
        mAssetResources.clear();
    }
NS_END
