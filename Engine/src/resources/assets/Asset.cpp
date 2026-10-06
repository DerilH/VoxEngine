//
// Created by deril on 2/19/26.
//

#include "VoxEngine/resources/assets/Asset.h"

#include <VoxCore/containers/Containers.h>

RESOURCES_NS
    Asset::Asset(InternedString path, Asset **nested, uint32_t nestedCount) : mPath(path), mNestedAssets(nested), mNestedAssetsCount(nestedCount) {
    }

    InternedString Asset::getPath() const {
        return mPath;
    }

    bool Asset::hasNested() const {
        return mNestedAssets != nullptr;
    }


    uint32_t Asset::getNestedCount() const {
        return mNestedAssetsCount;
    }

    UPtr<Asset> Asset::clone() const {
        VOX_CHECK_FMT(false, "{} asset cannot be clone", +type())
    }

    void Asset::cloneTo(void *ptr) const {
        VOX_CHECK_FMT(false, "{} asset cannot be clone", +type())
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
