//
// Created by deril on 2/19/26.
//

#pragma once
#include <expected>

#include "VoxEngine/resources/assets/Asset.h"
#include "VoxEngine/resources/assets/AssetLoader.h"
#include "VoxEngine/resources/assets/RegularFileLoader.h"
#include <string>
#include <unordered_map>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/ArrayView.h>

#include "AssetDirectory.h"
#include "assets/AssetWriter.h"
#include "assets/MaterialAsset.h"
#include "assets/MeshAsset.h"
#include "assets/ModelAsset.h"

RESOURCES_NS
    class Defaults;

    class ResourceManager {
        std::filesystem::path mResourcesRoot;
        UPtr<AssetDirectory> mRootDir;
        HashMap<InternedString, UPtr<Asset> > mAssets;
        Vector<InternedString> mDirty;
        static const UPtr<RegularFileLoader> sRegularLoader;
        static const HashMap<InternedString, UPtr<AssetLoader> > sLoaderByExtension;
        static const HashMap<AssetType, UPtr<AssetWriter> > sWriterByType;

        void loadDir(const std::filesystem::path &currentPath, AssetDirectory &currentDir);

    public:
        void loadAll();

        template<std::derived_from<Asset> T>
        Expected<ConstRef<T>> get(const InternedString key) const {
            auto asset = mAssets.find(key);
            VOX_EXPECT(asset != mAssets.end(), "Cannot find asset");
            VOX_EXPECT(asset->second->type() == T::StaticType(), "Asset type mismatch");
            return static_cast<T *>(asset->second.get());
        }

        template<std::derived_from<Asset> T>
        ConstRef<T> getDefault() const {
            static_assert(false, "No default value for provided asset type");
        }

        template<std::derived_from<Asset> T>
        UPtr<T> getForEdit(const InternedString key) const {
            return get<T>(key)->clone();
        }

        template<std::derived_from<Asset> T>
        void update(const InternedString key, UPtr<T> asset) {
            auto &original = mAssets[key];
            if (original == nullptr || original->type() != asset->type()) {
                original = std::move(asset);
            } else {
                asset->cloneTo(original.get());
            }
            mDirty.emplace_back(key);
        }

        void processDirty();

        ArrayView<void> readFile(std::filesystem::path path) const;

        ConstRef<AssetDirectory> getRootDir() const {
            return mRootDir.get();
        }

        Vector<ConstRef<Asset> > listDirAssets(InternedString path) const;

        void setRoot(std::filesystem::path path);
    };

    template<>
    inline ConstRef<MaterialAsset> ResourceManager::getDefault() const {
        return get<MaterialAsset>("defaults/DefaultMaterial.vmat").value();
    }

    template<>
    inline ConstRef<ModelAsset> ResourceManager::getDefault() const {
        return get<ModelAsset>("defaults/DefaultCube.fbx").value();
    }

NS_END
