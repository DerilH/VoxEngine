//
// Created by deril on 2/20/26.
//

#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Assert.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/math/UUID.h>

#include "ResourceType.h"
#include "ResourceHandle.h"
#include "ResourceSlotPool.h"
#include "MeshResource.h"
#include "ModelResource.h"
#include "MaterialResource.h"
#include "ShaderResource.h"
#include "TextureResource.h"

RESOURCES_NS
class ResourceManager {
    ResourceSlotPool<MeshResource> mMeshPool;
    ResourceSlotPool<ModelResource> mModelPool;
    ResourceSlotPool<MaterialResource> mMaterialPool;
    ResourceSlotPool<ShaderResource> mShaderPool;
    ResourceSlotPool<TextureResource> mTexturePool;

    HashMap<InternedString, ResourceHandleRaw> mNamedResources;
    HashMap<UUID, ResourceHandleRaw> mAssetResources;

public:
    ResourceManager() = default;

    ~ResourceManager() = default;

    NO_COPY_MOVE(ResourceManager);

    // Pool accessors
    template<typename T>
    ResourceSlotPool<T> &getPool();

    template<typename T>
    const ResourceSlotPool<T> &getPool() const;

    // Add resource with all validation checks
    template<typename T>
    Expected<ResourceHandle<T> > addResource(T resource, InternedString name = "") {
        auto valRes = resource.validate();
        VOX_EXPECT(valRes, valRes.error());

        if (!name.empty()) {
            auto it = mNamedResources.find(name);
            VOX_EXPECT(it == mNamedResources.end(), "Resource with name '{}' already exists", name.c_str());
        }

        auto allocRes = getPool<T>().allocate(std::move(resource));
        VOX_EXPECT(allocRes, allocRes.error());

        const ResourceHandle<T> handle = *allocRes;
        if (!name.empty()) {
            mNamedResources.emplace(name, handle);
        }
        return handle;
    }

    // Add resource linked to an authoring asset UUID
    template<typename T>
    Expected<ResourceHandle<T> > addResourceWithAsset(UUID assetUuid, T resource, InternedString name = "") {
        auto it = mAssetResources.find(assetUuid);
        if (it != mAssetResources.end()) {
            if (isValidRaw(it->second) && it->second.type == ResourceTraits<T>::Type) {
                return ResourceHandle<T>(it->second);
            }
        }

        auto handleRes = addResource<T>(std::move(resource), name);
        VOX_EXPECT(handleRes, handleRes.error());

        mAssetResources[assetUuid] = *handleRes;
        return *handleRes;
    }

    // Retrieve resource by handle
    template<typename T>
    Expected<Ref<T> > get(ResourceHandle<T> handle) {
        return getPool<T>().get(handle);
    }

    template<typename T>
    Expected<ConstRef<T> > get(ResourceHandle<T> handle) const {
        return getPool<T>().get(handle);
    }

    // Find handle by name
    template<typename T>
    Expected<ResourceHandle<T> > find(InternedString name) const {
        auto it = mNamedResources.find(name);
        VOX_EXPECT(it != mNamedResources.end(), "No resource found with name '{}'", name.c_str());
        VOX_EXPECT(it->second.type == ResourceTraits<T>::Type, "Resource type mismatch for '{}': expected {}, found {}", name.c_str(), +ResourceTraits<T>::Type, +it->second.type);
        VOX_EXPECT(isValidRaw(it->second), "Resource handle for '{}' is no longer valid", name.c_str());
        return ResourceHandle<T>(it->second);
    }

    // Find handle by asset UUID
    template<typename T>
    Expected<ResourceHandle<T> > findByAsset(UUID assetUuid) const {
        auto it = mAssetResources.find(assetUuid);
        VOX_EXPECT(it != mAssetResources.end(), "No resource associated with asset UUID {}", assetUuid.toString().c_str());
        VOX_EXPECT(it->second.type == ResourceTraits<T>::Type, "Resource type mismatch for asset UUID");
        VOX_EXPECT(isValidRaw(it->second), "Resource handle for asset UUID is no longer valid");
        return ResourceHandle<T>(it->second);
    }

    // Find raw handle
    std::optional<ResourceHandleRaw> findRaw(InternedString name) const {
        auto it = mNamedResources.find(name);
        if (it != mNamedResources.end() && isValidRaw(it->second)) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<ResourceHandleRaw> findByAssetRaw(UUID assetUuid) const {
        auto it = mAssetResources.find(assetUuid);
        if (it != mAssetResources.end() && isValidRaw(it->second)) {
            return it->second;
        }
        return std::nullopt;
    }

    // Release resource
    template<typename T>
    bool remove(ResourceHandle<T> handle) {
        if (!getPool<T>().release(handle)) {
            return false;
        }

        // Clean up name mapping
        for (auto it = mNamedResources.begin(); it != mNamedResources.end(); ++it) {
            if (it->second == handle) {
                mNamedResources.erase(it);
                break;
            }
        }

        // Clean up asset mapping
        for (auto it = mAssetResources.begin(); it != mAssetResources.end(); ++it) {
            if (it->second == handle) {
                mAssetResources.erase(it);
                break;
            }
        }
        return true;
    }

    // Validity checks
    template<typename T>
    bool isValid(ResourceHandle<T> handle) const noexcept {
        return getPool<T>().isValid(handle);
    }

    bool isValidRaw(ResourceHandleRaw raw) const noexcept;

    bool removeRaw(ResourceHandleRaw raw);

    void clear();
};

// Explicit specializations for getPool
template<>
inline ResourceSlotPool<MeshResource> &ResourceManager::getPool<MeshResource>() {
    return mMeshPool;
}

template<>
inline const ResourceSlotPool<MeshResource> &ResourceManager::getPool<MeshResource>() const {
    return mMeshPool;
}

template<>
inline ResourceSlotPool<ModelResource> &ResourceManager::getPool<ModelResource>() {
    return mModelPool;
}

template<>
inline const ResourceSlotPool<ModelResource> &ResourceManager::getPool<ModelResource>() const {
    return mModelPool;
}

template<>
inline ResourceSlotPool<MaterialResource> &ResourceManager::getPool<MaterialResource>() {
    return mMaterialPool;
}

template<>
inline const ResourceSlotPool<MaterialResource> &ResourceManager::getPool<MaterialResource>() const {
    return mMaterialPool;
}

template<>
inline ResourceSlotPool<ShaderResource> &ResourceManager::getPool<ShaderResource>() {
    return mShaderPool;
}

template<>
inline const ResourceSlotPool<ShaderResource> &ResourceManager::getPool<ShaderResource>() const {
    return mShaderPool;
}

template<>
inline ResourceSlotPool<TextureResource> &ResourceManager::getPool<TextureResource>() {
    return mTexturePool;
}

template<>
inline const ResourceSlotPool<TextureResource> &ResourceManager::getPool<TextureResource>() const {
    return mTexturePool;
}

NS_END
