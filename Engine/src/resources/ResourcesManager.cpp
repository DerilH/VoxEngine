//
// Created by deril on 2/19/26.
//

#include <filesystem>
#include "VoxEngine/resources/ResourcesManager.h"

#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <VoxEngine/resources/assets/MaterialLoader.h>
#include <VoxEngine/resources/assets/MaterialWriter.h>

#include "VoxEngine/resources/assets/FbxLoader.h"
#include "VoxEngine/resources/assets/ShaderLoader.h"

RESOURCES_NS
    class AssetWriter;

    const UPtr<RegularFileLoader> ResourcesManager::sRegularLoader = makeUPtr<RegularFileLoader>();
    const HashMap<InternedString, UPtr<AssetLoader> > ResourcesManager::sLoaderByExtension = []() {
        HashMap<InternedString, UPtr<AssetLoader> > map;
        map.emplace(".fbx", makeUPtr<FbxLoader>());
        map.emplace(".vert", makeUPtr<ShaderLoader>());
        map.emplace(".frag", makeUPtr<ShaderLoader>());
        map.emplace(".vmat", makeUPtr<MaterialLoader>());
        return map;
    }();

    const HashMap<AssetType, UPtr<AssetWriter> > ResourcesManager::sWriterByType = []() {
        HashMap<AssetType, UPtr<AssetWriter> > map;
        map.emplace(AssetType::MATERIAL, makeUPtr<MaterialWriter>());
        return map;
    }();

    void ResourcesManager::loadAll() {
        VOX_CHECK(!mResourcesRoot.empty(), "Resource root path not set");
        loadDir(mResourcesRoot, *mRootDir);
    }

    void ResourcesManager::loadDir(const std::filesystem::path &currentPath, AssetDirectory &currentDir) {
        for (const auto &entry: std::filesystem::directory_iterator(currentPath)) {
            const auto &path = entry.path();
            auto relPath = relative(path, mResourcesRoot);
            if (!entry.is_regular_file()) {
                if (entry.is_directory()) {
                    auto parentRel = relative(path.parent_path(), mResourcesRoot);
                    auto dir = makeUPtr<AssetDirectory>(relPath.string(), path.filename());
                    loadDir(path, *dir);
                    currentDir.addDirectory(std::move(dir));
                }
                continue;
            }
            auto it = sLoaderByExtension.find(path.extension());
            const auto loader = it == sLoaderByExtension.end() ? sRegularLoader.get() : it->second.get();
            try {
                auto view = readFile(path);
                Asset *asset = loader->load(relPath, view);
                mAssets.emplace(relPath.string(), asset);
            } catch (std::exception &e) {
                LOG_ERROR("Error while loading asset: {} {}", path.string(), e.what())
            }
        }
    }

    void ResourcesManager::processDirty() {
        VOX_CHECK(!mResourcesRoot.empty(), "Resource root path not set");

        for (const auto &key: mDirty) {
            auto &asset = mAssets[key];
            const auto &writer = sWriterByType.find(asset->type());
            if (writer == sWriterByType.end()) {
                LOG_WARN("No writer provided for asset: {} of type: {}. Skipped", asset->getPath(), magic_enum::enum_name<>(asset->type()));
            } else if (asset) {
                writer->second->writeToFile(mResourcesRoot / asset->getPath(), asset.get());
            }
        }
        mDirty.clear();
    }

    ArrayView<void> ResourcesManager::readFile(std::filesystem::path path) const {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
            return ArrayView<void>(nullptr, 0);

        std::streamsize size = std::filesystem::file_size(path);
        if (size <= 0)
            return ArrayView<void>(nullptr, 0);

        file.seekg(0, std::ios::beg);

        void *buffer = malloc(size);
        if (!buffer)
            return ArrayView<void>(nullptr, 0);

        if (!file.read(static_cast<char *>(buffer), size)) {
            free(buffer);
            return ArrayView<void>(nullptr, 0);
        }

        const auto outSize = static_cast<size_t>(size);
        return ArrayView(buffer, outSize);
    }

    Vector<ConstRef<Asset> > ResourcesManager::listDirAssets(InternedString path) const {
        static const Vector<ConstRef<Asset> > emptyVector(0);
        auto dirPath = mResourcesRoot / path;
        if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) return emptyVector;

        Vector<ConstRef<Asset> > out;
        for (const auto &entry: std::filesystem::directory_iterator(dirPath)) {
            if (!entry.is_regular_file()) continue;

            const auto &p = entry.path();
            auto rel = relative(p, mResourcesRoot);
            try {
                ConstRef<Asset> r = mAssets.at(rel).get();
                out.emplace_back(r);
            } catch (std::exception &e) {
            }
        }
        return out;
    }


    void ResourcesManager::SetRoot(std::filesystem::path path) {
        VOX_CHECK(is_directory(path), "Resource root path must be a directory");
        VOX_CHECK(exists(path), "Resource root not exists");
        Get().mRootDir = makeUPtr<AssetDirectory>(".", path.filename());
        Get().mResourcesRoot = std::move(path);
        LOG_INFO("Resource manager root: {}", absolute(Get().mResourcesRoot).lexically_normal().string());
    }

NS_END
