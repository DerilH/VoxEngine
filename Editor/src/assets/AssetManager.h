//
// Created by deril on 2/19/26.
//

#pragma once
#include <expected>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/containers/ArrayView.h>
#include <VoxCore/math/UUID.h>
#include <VoxCore/Assert.h>

#include "AssetDirectory.h"
#include "variants/Asset.h"
#include "variants/MaterialAsset.h"
#include "variants/MeshAsset.h"
#include "variants/ModelAsset.h"
#include "variants/ShaderAsset.h"
#include "variants/TextureAsset.h"
#include "variants/RegularFileAsset.h"
#include "variants/import/AssetCache.h"
#include "variants/import/AssetMetadata.h"
#include "variants/import/AssetImporter.h"
#include "variants/import/RegularFileImporter.h"
#include "variants/write/AssetWriter.h"

ASSETS_NS
    class AssetWriter;
    class Defaults;

    inline Expected<ArrayView<void> > readFileData(FILE *file) {
        VOX_EXPECT(std::fseek(file, 0, SEEK_END) == 0, "Cant seek file {}", std::strerror(errno));

        long fileSize = std::ftell(file);
        if (fileSize < 0) {
            fclose(file);
            VOX_EXPECT(false, "Cant read file {}", std::strerror(errno));
        }

        std::rewind(file);

        auto data = new char8_t[fileSize];
        if (fileSize > 0) {
            const auto bytesRead = std::fread(data, 1, fileSize, file);
            if (bytesRead != static_cast<size_t>(fileSize)) {
                delete[] data;
                VOX_EXPECT(false, "Cant fully read file {}", std::strerror(errno));
            }
        }
        return ArrayView(static_cast<void *>(data), fileSize);
    }

    class AssetManager {
        std::filesystem::path mAssetsRoot;
        std::filesystem::path mAssetsCache;
        UPtr<AssetDirectory> mRootDir;
        HashMap<UUID, UPtr<Asset> > mLoadedAssets;
        HashMap<UUID, UPtr<AssetMetadata> > mAssetsMetadata;

        HashMap<InternedString, UUID> mPathToUUID;
        HashMap<UUID, InternedString> mUUIDToPath;

        Vector<UUID> mDirty;
        static const UPtr<RegularFileImporter> sRegularLoader;
        static const HashMap<InternedString, UPtr<AssetImporter> > sImporterByExtension;
        static const HashMap<AssetType, UPtr<AssetWriter> > sWriterByType;

        void loadDir(const std::filesystem::path &currentPath, AssetDirectory &currentDir);

        bool updateMetadataCache(UUID metadata, const std::filesystem::path &assetPath);

        void writeCache(UUID asset, const AssetCache &cache) const;

        Expected<AssetCache> readCache(UUID asset) const;

        Ref<AssetImporter> getImporterOrDefault(InternedString extension) const;

    public:
        AssetManager() = default;

        ~AssetManager() = default;

        void loadAll();

        // Querying by UUID
        template<std::derived_from<Asset> T>
        Expected<ConstRef<T> > getOrLoad(const UUID key) const {
            auto it = mLoadedAssets.find(key);
            VOX_EXPECT(it != mLoadedAssets.end(), "Cannot find asset with UUID: {}", key);
            VOX_EXPECT(it->second->type() == T::StaticType(), "Asset type mismatch");
            return static_cast<ConstRef<T>>(it->second.get());
        }

        // Querying by path
        template<std::derived_from<Asset> T>
        Expected<ConstRef<T> > findByPath(const InternedString path) const {
            auto it = mPathToUUID.find(path);
            if (it != mPathToUUID.end()) {
                return getOrLoad<T>(it->second);
            }
            VOX_EXPECT(false, "Cannot find asset: {}", path.c_str());
        }

        ConstRef<Asset> getAsset(const UUID key) const {
            auto it = mLoadedAssets.find(key);
            return it != mLoadedAssets.end() ? it->second.get() : nullptr;
        }

        template<std::derived_from<Asset> T>
        ConstRef<T> getDefault() const {
            static_assert(sizeof(T) == 0, "No default value for provided asset type");
        }

        template<std::derived_from<Asset> T>
        UPtr<T> getForEdit(const UUID key) const {
            auto res = getOrLoad<T>(key);
            if (!res) return nullptr;
            return (*res)->cloneTyped();
        }

        template<std::derived_from<Asset> T>
        UPtr<T> getForEdit(const InternedString key) const {
            auto res = findByPath<T>(key);
            if (!res) return nullptr;
            return (*res)->cloneTyped();
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

        template<std::derived_from<Asset> T>
        void update(const InternedString key, UPtr<T> asset) {
            auto it = mPathToUUID.find(key);
            if (it != mPathToUUID.end()) {
                update<T>(it->second, std::move(asset));
            }
        }

        void processDirty();

        ConstRef<AssetDirectory> getRootDir() const {
            return mRootDir.get();
        }

        const std::filesystem::path &getResourcesRoot() const {
            return mAssetsRoot;
        }

        const HashMap<UUID, UPtr<Asset> > &getLoadedAssets() const {
            return mLoadedAssets;
        }

        Vector<UUID> listDirAssets(InternedString path) const;

        void setPaths(std::filesystem::path path, std::filesystem::path cachePath);

        UUID getUUID(InternedString path) const {
            auto it = mPathToUUID.find(path);
            return it != mPathToUUID.end() ? it->second : UUID();
        }

        InternedString getPath(UUID uuid) const {
            auto it = mUUIDToPath.find(uuid);
            return it != mUUIDToPath.end() ? it->second : InternedString();
        }

        ConstRef<AssetMetadata> getMetadata(UUID uuid) const {
            auto it = mAssetsMetadata.find(uuid);
            return it != mAssetsMetadata.end() ? it->second.get() : nullptr;
        }
    };

    template<>
    inline ConstRef<MaterialAsset> AssetManager::getDefault() const {
        return findByPath<MaterialAsset>("defaults/DefaultMaterial.vmat").value();
    }

    template<>
    inline ConstRef<ModelAsset> AssetManager::getDefault() const {
        return findByPath<ModelAsset>("defaults/DefaultCube.fbx").value();
    }

NS_END
