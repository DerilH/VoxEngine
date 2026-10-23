//
// Created by deril on 2/19/26.
//

#include <filesystem>
#include "AssetManager.h"

#include <magic_enum/magic_enum.hpp>
#include <memory>
#define XXH_INLINE_ALL
#include <xxhash.h>
#include <functional>

#include "variants/import/AssetMetadata.h"
#include "variants/import/FbxImporter.h"
#include "variants/import/MaterialImporter.h"
#include "variants/import/ShaderImporter.h"
#include "variants/import/TextureImporter.h"
#include "variants/import/RegularFileImporter.h"
#include "variants/write/MaterialWriter.h"

ASSETS_NS
    Expected<FILE *> openFileRead(const char *path) {
        auto file = std::fopen(path, "rb");
        VOX_EXPECT(file != nullptr, "Cannot open file for read", std::strerror(errno));
        return file;
    }

    Expected<FILE *> openFileWrite(const char *path) {
        auto file = std::fopen(path, "wb");
        VOX_EXPECT(file != nullptr, "Cannot open file for write", std::strerror(errno));
        return file;
    }

    inline uint64_t readTime(const std::filesystem::path &path) {
        const auto ftime = std::filesystem::last_write_time(path);
        const auto sys_time = std::chrono::file_clock::to_sys(ftime);

        return std::chrono::duration_cast<std::chrono::seconds>(
            sys_time.time_since_epoch()
        ).count();
    }

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
        map.emplace(AssetType::MATERIAL, makeUPtr<MaterialWriter>());
        return map;
    }();

    Ref<AssetImporter> AssetManager::getImporterOrDefault(InternedString extension) const {
        const auto importer = sImporterByExtension.find(extension);
        return importer == sImporterByExtension.end() ? sRegularLoader.get() : importer->second.get();
    }

    Expected<AssetCache> AssetManager::readCache(UUID asset) const {
        auto path = mAssetsCache / asset.toString();
        auto file = openFileRead(path.c_str());
        VOX_EXPECT(file, "");
        auto cache = AssetCache::deserialize(*file);
        fclose(*file);
        return cache;
    }

    void AssetManager::writeCache(const UUID asset, const AssetCache &cache) const {
        const auto path = mAssetsCache / asset.toString();
        auto file = openFileWrite(path.c_str());
        if (!file) {
            LOG_WARN("Cannot write asset cache {}, cause: {}", path.c_str(), file.error().format());
            return;
        }

        cache.serialize(*file);
        fclose(*file);
    }

    void AssetManager::loadAll() {
        VOX_CHECK(!mAssetsRoot.empty(), "Resource root path not set");
        loadDir(mAssetsRoot, *mRootDir);
    }

    Expected<AssetCache> createCache(const std::filesystem::path &path) {
        const auto size = std::filesystem::file_size(path);
        const auto mtime = readTime(path);
        auto file = openFileRead(path.c_str());
        VOX_EXPECT(file, file.error());
        const auto data = readFileData(*file);
        VOX_EXPECT(data, data.error());
        const auto [low64, high64] = XXH3_128bits(data->pData, data->size());
        delete[] static_cast<char8_t *>(data->pData);
        return AssetCache{Hash128(high64, low64), mtime, size};
    }

    bool AssetManager::updateMetadataCache(UUID metadata, const std::filesystem::path &assetPath) {
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
        namespace fs = std::filesystem;
        if (!fs::exists(currentPath) || !fs::is_directory(currentPath)) return;

        for (const auto &entry: fs::directory_iterator(currentPath)) {
            const auto &path = entry.path();

            if (entry.is_directory()) {
                if (path.filename() == ".cache" || path.filename().string().starts_with(".")) {
                    continue;
                }
                auto relPath = fs::relative(path, mAssetsRoot);
                auto dir = makeUPtr<AssetDirectory>(relPath.string(), path.filename().string());
                loadDir(path, *dir);
                currentDir.addDirectory(std::move(dir));
                continue;
            }

            if (!entry.is_regular_file()) continue;

            auto extension = path.extension();
            if (extension == ".meta") {
                auto base = path.stem();
                auto assetPath = path.parent_path() / base;
                if (!fs::exists(assetPath)) {
                    LOG_WARN("Alone asset metadata found: {}", path.c_str());
                }
                continue;
            }

            auto relPath = fs::relative(path, mAssetsRoot);
            fs::path metaPath = path;
            metaPath += ".meta";

            UPtr<AssetMetadata> metadata = nullptr;
            if (fs::exists(metaPath)) {
                if (auto metaFile = openFileRead(metaPath.c_str())) {
                    auto res = AssetMetadata::deserialize(*metaFile);
                    fclose(*metaFile);
                    if (res) {
                        metadata = std::move(*res);
                    } else {
                        LOG_WARN("Corrupted metadata at {}, regenerating", metaPath.c_str());
                    }
                }
            }

            auto importer = getImporterOrDefault(extension);
            if (!metadata) {
                UUID uuid = UUID::uuid4();
                AssetType type = importer->type();

                metadata = makeUPtr<AssetMetadata>(uuid, 1, type, nullptr);
                auto metaWriteFile = openFileWrite(metaPath.c_str());
                if (metaWriteFile) {
                    metadata->serialize(*metaWriteFile);
                    fclose(*metaWriteFile);
                } else {
                    LOG_WARN("Cannot write metadata to {}: {}", metaPath.c_str(), metaWriteFile.error().format());
                }
            }

            UUID assetUuid = metadata->uuid;
            currentDir.addAsset(assetUuid);

            updateMetadataCache(assetUuid, path);

            //Loading
            try {
                if (auto fileData = openFileRead(path.c_str())) {
                    Ref<Asset> asset = importer->load(*fileData);
                    if (asset) {
                        mLoadedAssets.emplace(assetUuid, asset);
                        mPathToUUID[relPath.string()] = assetUuid;
                        mUUIDToPath[assetUuid] = relPath.string();

                        for (uint32_t i = 0; i < asset->getNestedCount(); i++) {
                            auto nested = asset->getNested()[i];
                            if (nested) {
                                mNestedAssets[nested->getPath()] = nested;
                            }
                        }
                    }
                } else {
                    LOG_ERROR("Failed to read asset file {}: {}", path.c_str(), fileData.error().format());
                }
            } catch (const std::exception &e) {
                LOG_ERROR("Exception while loading asset {}: {}", path.c_str(), e.what());
            }

            mAssetsMetadata.emplace(assetUuid, std::move(metadata));
        }
    }

    void AssetManager::processDirty() {
        VOX_CHECK(!mAssetsRoot.empty(), "Resource root path not set");

        for (const auto &key: mDirty) {
            auto it = mLoadedAssets.find(key);
            if (it == mLoadedAssets.end() || !it->second) continue;
            const auto &asset = it->second;
            const auto &writer = sWriterByType.find(asset->type());
            if (writer == sWriterByType.end()) {
                LOG_WARN("No writer provided for asset: {} of type: {}. Skipped", asset->getPath().c_str(), magic_enum::enum_name<>(asset->type()));
            } else {
                writer->second->writeToFile(mAssetsRoot / asset->getPath().c_str(), asset.get());
                updateMetadataCache(key, mAssetsRoot / asset->getPath().c_str());
            }
        }
        mDirty.clear();
    }

    Expected<ArrayView<void> > AssetManager::readFile(const std::filesystem::path &path) const {
        return readFileData(path);
    }

    Vector<ConstRef<Asset> > AssetManager::listDirAssets(InternedString path) const {
        Vector<ConstRef<Asset> > out;
        std::function<ConstRef<AssetDirectory>(ConstRef<AssetDirectory>, const std::string &)> findDir =
                [&](ConstRef<AssetDirectory> node, const std::string &target) -> ConstRef<AssetDirectory> {
            if (!node) return nullptr;
            if (node->path == target || (target == "." && node->path == ".")) return node;
            for (const auto &child: node->getNested()) {
                auto found = findDir(child.get(), target);
                if (found) return found;
            }
            return nullptr;
        };

        std::string target = path.c_str();
        if (target.empty()) target = ".";

        auto dir = findDir(mRootDir.get(), target);
        if (dir) {
            for (const auto &uuid: dir->getAssets()) {
                auto it = mLoadedAssets.find(uuid);
                if (it != mLoadedAssets.end() && it->second) {
                    out.emplace_back(it->second.get());
                }
            }
        }
        return out;
    }

    void AssetManager::setPaths(std::filesystem::path path, std::filesystem::path cachePath) {
        VOX_CHECK(std::filesystem::is_directory(path), "Assets root path must be a directory");
        VOX_CHECK(std::filesystem::exists(path), "Assets root not exists");
        mRootDir = makeUPtr<AssetDirectory>(".", path.filename().string());
        mAssetsRoot = std::move(path);
        LOG_INFO("Assets root: {}", std::filesystem::absolute(mAssetsRoot).lexically_normal().string());

        VOX_CHECK(cachePath.filename() == ".cache", "Cache dir name does not equal to '.cache'");

        if (!std::filesystem::exists(cachePath)) {
            VOX_CHECK_FMT(std::filesystem::create_directories(cachePath), "Cannot create cache dir {}", cachePath.c_str());
        }
        mAssetsCache = std::move(cachePath);
        LOG_INFO("Assets cache path: {}", std::filesystem::absolute(mAssetsCache).lexically_normal().string());
    }

NS_END
