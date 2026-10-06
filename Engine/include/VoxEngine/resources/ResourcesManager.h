//
// Created by deril on 2/19/26.
//

#pragma once
#include "VoxEngine/resources/assets/Asset.h"
#include "VoxEngine/resources/assets/AssetLoader.h"
#include "VoxEngine/resources/assets/RegularFileLoader.h"
#include "VoxCore/SingletonBase.h"
#include <string>
#include <unordered_map>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/ArrayView.h>

#include "assets/AssetWriter.h"

RESOURCES_NS
    class ResourcesManager : public SingletonBase<ResourcesManager> {
        std::filesystem::path mResourcesRoot;
        HashMap<InternedString, UPtr<Asset> > mAssets;
        Vector<InternedString> mDirty;
        static const UPtr<RegularFileLoader> sRegularLoader;
        static const HashMap<InternedString, UPtr<AssetLoader> > sLoaderByExtension;
        static const HashMap<AssetType, UPtr<AssetWriter> > sWriterByType;

    public:
        void loadAll();

        template<typename T>
            requires std::derived_from<T, Asset>
        ConstRef<T> get(const InternedString key) const {
            auto asset = mAssets.find(key);
            VOX_CHECK_FMT(asset != mAssets.end(), "Cannot find asset {}", key);
            VOX_CHECK_FMT(asset->second->type() == T::StaticType(), "Asset type mismatch for key {}. Actual type is {}. Provided type {}", key, +asset->second->type(), +T::StaticType());

            return static_cast<T *>(asset->second.get());
        }

        template<typename T>
            requires std::derived_from<T, Asset>
        UPtr<T> getForEdit(const InternedString key) const {
            return get<T>(key)->clone();
        }

        template<typename T>
            requires std::derived_from<T, Asset>
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
        static void SetRoot(std::filesystem::path path);
    };

NS_END
