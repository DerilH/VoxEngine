//
// Created by deril on 2/19/26.
//

#pragma once
#include <expected>

#include <string>
#include <unordered_map>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/ArrayView.h>
#include <VoxCore/math/UUID.h>

#include "AssetDirectory.h"
#include "variants/MaterialAsset.h"
#include "variants/MeshAsset.h"
#include "variants/import/AssetCache.h"
#include "variants/import/AssetMetadata.h"
#include "variants/import/RegularFileImporter.h"
#include "variants/write/AssetWriter.h"

ASSETS_NS
    class AssetWriter;
    class Defaults;

    class AssetManager {
        std::filesystem::path mAssetsRoot;
        std::filesystem::path mAssetsCache;
        UPtr<AssetDirectory> mRootDir;
        HashMap<UUID, UPtr<Asset>> mLoadedAssets;
        HashMap<UUID, UPtr<AssetMetadata>> mAssetsMetadata;

        Vector<UUID> mDirty;
        static const UPtr<RegularFileImporter> sRegularLoader;
        static const HashMap<InternedString, UPtr<AssetImporter> > sImporterByExtension;
        static const HashMap<AssetType, UPtr<AssetWriter> > sWriterByType;

        void loadDir(const std::filesystem::path &currentPath, AssetDirectory &currentDir);

        bool updateMetadata(UUID metadata, const std::filesystem::path &assetPath);
        void writeCache(UUID asset, const AssetCache &cache) const;
        Expected<AssetCache> readCache(UUID asset) const;
    public:
        void loadAll();

        template<std::derived_from<Asset> T>
        Expected<ConstRef<T>> get(const UUID key) const {
            auto asset = mLoadedAssets.find(key);
            VOX_EXPECT(asset != mLoadedAssets.end(), "Cannot find asset: {}", key);
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
        void update(const UUID key, UPtr<T> asset) {
            auto &original = mLoadedAssets[key];
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

        const std::filesystem::path &getResourcesRoot() const {
            return mAssetsRoot;
        }

        Vector<ConstRef<Asset> > listDirAssets(InternedString path) const;

        void setPaths(std::filesystem::path path, std::filesystem::path cachePath);
    };



    // template<>
    // inline ConstRef<MaterialAsset> AssetManager::getDefault() const {
        // return get<MaterialAsset>("defaults/DefaultMaterial.vmat").value();
    // }

    // template<>
    // inline ConstRef<ModelAsset> AssetManager::getDefault() const {
        // return get<ModelAsset>("defaults/DefaultCube.fbx").value();
    // }

NS_END
