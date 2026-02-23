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

RESOURCES_NS
    class ResourcesManager : public SingletonBase<ResourcesManager> {
        std::filesystem::path mResourcesRoot;
        std::unordered_map<std::string, Asset *> mAssets;
        static const RegularFileLoader sRegularLoader;
        static const std::unordered_map<std::string, AssetLoader *> sLoaderByExtension;

    public:
        void loadAll();

        template<typename T, typename = std::enable_if_t<std::is_base_of_v<Asset, T>>>
        T * get(const std::string &key) const {
            return (T*)mAssets.at(key);
        }

        static void SetRoot(std::filesystem::path path);
    };
NS_END