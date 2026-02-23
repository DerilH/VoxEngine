//
// Created by deril on 2/19/26.
//

#include <utility>

#include "VoxEngine/resources/assets/Asset.h"

RESOURCES_NS
    Asset::Asset(std::string path, Asset **nested, uint32_t nestedCount) : mPath(std::move(path)), mNestedAssets(nested), mNestedAssetsCount(nestedCount) {

    }

    std::string Asset::getPath() const {
        return mPath;
    }

    bool Asset::hasNested() const {
        return mNestedAssets != nullptr;
    }


    uint32_t Asset::getNestedCount() const {
        return mNestedAssetsCount;
    }

    void Asset::serialize(FILE *file) {
        serializeInternal(file, type());
        serializeInternal(file, mPath);
        if (hasNested()) {
            serializeInternal(file, mNestedAssetsCount);
            for (int i = 0; i < mNestedAssetsCount; ++i) {
                mNestedAssets[i]->serialize(file);
            }
        }
    }

    Asset::~Asset() = default;
NS_END

