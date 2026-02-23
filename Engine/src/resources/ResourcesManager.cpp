//
// Created by deril on 2/19/26.
//

#include <filesystem>
#include "VoxEngine/resources/ResourcesManager.h"
#include "VoxEngine/resources/assets/FbxLoader.h"
#include "VoxEngine/resources/assets/ShaderLoader.h"

RESOURCES_NS
    const RegularFileLoader ResourcesManager::sRegularLoader{};
    const std::unordered_map<std::string, AssetLoader *> ResourcesManager::sLoaderByExtension = {
            {".fbx", new FbxLoader()},
            {".vert", new ShaderLoader()},
            {".frag", new ShaderLoader()}

    };

    void ResourcesManager::loadAll() {
        VOX_CHECK(!mResourcesRoot.empty(), "Resource root path not set");
        VOX_CHECK(is_directory(mResourcesRoot), "Resource root path must be a directory");
        VOX_CHECK(exists(mResourcesRoot), "Resource root not exists");

        for (const auto &entry: std::filesystem::recursive_directory_iterator(mResourcesRoot)) {
            if (!entry.is_regular_file()) continue;
            const auto &path = entry.path();
            auto it = sLoaderByExtension.find(path.extension());
            AssetLoader *loader = it == sLoaderByExtension.end() ? (AssetLoader *) &sRegularLoader : it->second;
            try {
                Asset *asset = loader->fromFile(path.string());
                auto rel = relative(path, mResourcesRoot);
                mAssets.emplace(rel.string(), asset);
            } catch (std::exception &e) {
                LOG_ERROR("Error while loading asset: {} {}", path.string(), e.what())
            }
        }
    }

    void ResourcesManager::SetRoot(std::filesystem::path path) {
        Get().mResourcesRoot = std::move(path);
        LOG_INFO("Resource manager root: {}", absolute(Get().mResourcesRoot).lexically_normal().string());
    }
NS_END

