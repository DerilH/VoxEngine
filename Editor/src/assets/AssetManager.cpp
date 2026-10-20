//
// Created by deril on 2/19/26.
//

#include <filesystem>
#include "AssetManager.h"

#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <xxhash.h>

#include "variants/import/AssetMetadata.h"
#include "variants/import/FbxImporter.h"
#include "variants/import/MaterialImporter.h"
#include "variants/import/ShaderImporter.h"
#include "variants/import/TextureImporter.h"
#include "variants/write/MaterialWriter.h"

ASSETS_NS
    Expected<FILE *> openFileRead(const char *path) {
        auto file = std::fopen(path, "rb");
        VOX_EXPECT(file, "Cannot open file for read", std::strerror(errno));
        return file;
    }

    Expected<FILE *> openFileWrite(const char *path) {
        auto file = std::fopen(path, "w");
        VOX_EXPECT(file, "Cannot open file for write", std::strerror(errno));
        return file;
    }

    Expected<ArrayView<void> > readFile(const std::filesystem::path &path) {
        auto file = openFileRead(path.c_str());
        VOX_EXPECT(file, file.error());
        VOX_EXPECT(std::fseek(*file, 0, SEEK_END), "Cant seek file {}", std::strerror(errno));

        long fileSize = std::ftell(*file);
        if (fileSize < 0) {
            VOX_EXPECT(false, "Cant read file {}", std::strerror(errno))
        }

        std::rewind(*file);

        auto data = new char8_t[fileSize];
        if (fileSize > 0) {
            const auto bytesRead = std::fread(data, 1, fileSize, *file);
            VOX_EXPECT(bytesRead == fileSize, "Cant fully read file {}", std::strerror(errno));
        }

        fclose(*file);
        return ArrayView(static_cast<void *>(data), fileSize);
    }

    inline uint64_t readTime(const std::filesystem::path &path) {
        const auto ftime = std::filesystem::last_write_time(path);
        const auto sys_time = std::chrono::file_clock::to_sys(ftime);

        return std::chrono::duration_cast<std::chrono::seconds>(
            sys_time.time_since_epoch()
        ).count();
    }

    class AssetWriter;

    const UPtr<RegularFileImporter> AssetManager::sRegularLoader = makeUPtr<RegularFileImporter>();

    const HashMap<InternedString, UPtr<AssetImporter> > AssetManager::sImporterByExtension = []() {
        HashMap<InternedString, UPtr<AssetImporter> > map;
        map.emplace(".fbx", makeUPtr<FbxImporter>());
        map.emplace(".glb", makeUPtr<FbxImporter>());
        map.emplace(".vert", makeUPtr<ShaderImporter>());
        map.emplace(".frag", makeUPtr<ShaderImporter>());
        map.emplace(".vmat", makeUPtr<MaterialImporter>());
        map.emplace(".png", makeUPtr<TextureImporter>());
        map.emplace(".jpg", makeUPtr<TextureImporter>());
        return map;
    }();

    const HashMap<AssetType, UPtr<AssetWriter> > AssetManager::sWriterByType = []() {
        HashMap<AssetType, UPtr<AssetWriter> > map;
        // map.emplace(AssetType::MATERIAL, makeUPtr<MaterialWriter>());
        return map;
    }();

    Expected<AssetCache> AssetManager::readCache(UUID asset) const {
        auto path = mAssetsCache / asset.toString();
        auto file = openFileRead(path.c_str());
        VOX_EXPECT(file, "");
        return AssetCache::deserialize(*file);
    }

    void AssetManager::writeCache(const UUID asset, const AssetCache &cache) const {
        const auto path = mAssetsCache / asset.toString();
        auto file = openFileWrite(path.c_str());
        if (!file) {
            LOG_WARN("Cannot write asset cache {}, cause:", path.c_str(), file.error().format());
            return;
        }


        cache.serialize(*file);
    }


    void AssetManager::loadAll() {
        VOX_CHECK(!mAssetsRoot.empty(), "Resource root path not set");
        loadDir(mAssetsRoot, *mRootDir);
    }

    bool isMetadata(const std::filesystem::path &extension) {
        const auto &str = extension.native();
        const auto size = str.size();

        if (size < 5) return false;

        using CharT = std::filesystem::path::value_type;

        return str[size - 5] == static_cast<CharT>('.') &&
               str[size - 4] == static_cast<CharT>('m') &&
               str[size - 3] == static_cast<CharT>('e') &&
               str[size - 2] == static_cast<CharT>('t') &&
               str[size - 1] == static_cast<CharT>('a');
    }

    Expected<AssetCache> createCache(const std::filesystem::path &path) {
        const auto size = std::filesystem::file_size(path);
        const auto mtime = readTime(path);
        const auto file = readFile(path);
        VOX_EXPECT(file, file.error());
        const auto [low64, high64] = XXH3_128bits(file->pData, file->size());
        return AssetCache{Hash128(high64, low64), mtime, size};
    }

    bool AssetManager::updateMetadata(UUID metadata, const std::filesystem::path &assetPath) {
        namespace fs = std::filesystem;
        auto cache = readCache(metadata);

        auto needsUpdate = false;
        if (!cache || cache->size != fs::file_size(assetPath)) {
            needsUpdate = true;
        } else {
            needsUpdate = readTime(assetPath) != cache->mtime;
        }

        if (needsUpdate) {
            if (const auto c = createCache(assetPath)) {
                writeCache(metadata, *c);
                return true;
            }
        }
        return false;
    }

    void AssetManager::loadDir(const std::filesystem::path &currentPath, AssetDirectory &currentDir) {
        for (const auto &entry: std::filesystem::directory_iterator(currentPath)) {
            const auto &path = entry.path();
            auto relPath = relative(path, mAssetsRoot);
            if (!entry.is_regular_file()) {
                if (entry.is_directory()) {
                    auto parentRel = relative(path.parent_path(), mAssetsRoot);
                    auto dir = makeUPtr<AssetDirectory>(relPath.string(), path.filename());
                    loadDir(path, *dir);
                    currentDir.addDirectory(std::move(dir));
                }
                continue;
            }

            auto extension = path.extension();
            if (isMetadata(extension)) {
                auto base = path.stem();
                auto assetPath = path.parent_path() / base;
                if (!std::filesystem::exists(assetPath)) {
                    LOG_WARN("Alone asset metadata found {}", path.c_str());
                    // std::filesystem::remove(assetPath);
                    continue;
                }


                auto file = openFileRead(path.c_str());
                if (!file) {
                    LOG_WARN("Cannot load asset metadata {}: {}", path.c_str(), file.error().format())
                    continue;
                }
                const auto ptr = AssetMetadata::deserialize(file.value());
                if (!ptr) {
                    LOG_WARN("Cannot load asset metadata {}, : {}", path.c_str(), file.error().format())
                }

                if (!isMetadataValid())

                    mAssetsMetadata.emplace((*ptr)->uuid, *ptr);
            }
        }
        // for (const auto &entry: std::filesystem::directory_iterator(currentPath)) {
        //     const auto &path = entry.path();
        //     auto relPath = relative(path, mAssetsRoot);
        //     if (!entry.is_regular_file()) {
        //         if (entry.is_directory()) {
        //             auto parentRel = relative(path.parent_path(), mAssetsRoot);
        //             auto dir = makeUPtr<AssetDirectory>(relPath.string(), path.filename());
        //             loadDir(path, *dir);
        //             currentDir.addDirectory(std::move(dir));
        //         }
        //         continue;
        //     }
        //     auto it = sImporterByExtension.find(path.extension());
        //     const auto loader = it == sImporterByExtension.end() ? sRegularLoader.get() : it->second.get();
        //     try {
        //         auto view = readFile(path);
        //         Asset *asset = loader->load(relPath, view);
        //         mLoadedAssets.emplace(relPath.string(), asset);
        //         for (int i = 0; i < asset->getNestedCount(); i++) {
        //             auto nested = asset->getNested()[i];
        //             mLoadedAssets.emplace(nested->getPath(), nested);
        //         }
        //     } catch (std::exception &e) {
        //         LOG_ERROR("Error while loading asset: {} {}", path.string(), e.what())
        //     }
        // }
    }

    void AssetManager::processDirty() {
        VOX_CHECK(!mAssetsRoot.empty(), "Resource root path not set");

        for (const auto &key: mDirty) {
            auto &asset = mLoadedAssets[key];
            const auto &writer = sWriterByType.find(asset->type());
            if (writer == sWriterByType.end()) {
                LOG_WARN("No writer provided for asset: {} of type: {}. Skipped", asset->getPath(), magic_enum::enum_name<>(asset->type()));
            } else if (asset) {
                writer->second->writeToFile(mAssetsRoot / asset->getPath(), asset.get());
            }
        }
        mDirty.clear();
    }


    Vector<ConstRef<Asset> > AssetManager::listDirAssets(InternedString path) const {
        static const Vector<ConstRef<Asset> > emptyVector(0);
        auto dirPath = mAssetsRoot / path;
        if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) return emptyVector;

        Vector<ConstRef<Asset> > out;
        for (const auto &entry: std::filesystem::directory_iterator(dirPath)) {
            if (!entry.is_regular_file()) continue;

            const auto &p = entry.path();
            auto rel = relative(p, mAssetsRoot);
            try {
                ConstRef<Asset> r = mLoadedAssets.at(rel).get();
                out.emplace_back(r);
            } catch (std::exception &e) {
            }
        }
        return out;
    }

    void AssetManager::setPaths(std::filesystem::path path, std::filesystem::path cachePath) {
        VOX_CHECK(is_directory(path), "Assets root path must be a directory");
        VOX_CHECK(exists(path), "Assets root not exists");
        mRootDir = makeUPtr<AssetDirectory>(".", path.filename());
        mAssetsRoot = std::move(path);
        LOG_INFO("Assets root: {}", std::filesystem::absolute(mAssetsRoot).lexically_normal().string());

        VOX_CHECK(cachePath.filename() == ".cache", "Cache dir name does not equal to '.cache'");

        if (!std::filesystem::exists(cachePath)) {
            VOX_CHECK_FMT(std::filesystem::create_directories(cachePath), "Cannot create cache dir {}", cachePath.c_str())
        }
        mAssetsCache = cachePath;
        LOG_INFO("Assets cache path: {}", std::filesystem::absolute(mAssetsRoot).lexically_normal().string());
    }

NS_END
