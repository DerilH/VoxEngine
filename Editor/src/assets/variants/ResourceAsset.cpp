//
// Created by deril on 2/19/26.
//

#include "Asset.h"
#include <filesystem>
#include <VoxCore/Assert.h>

ASSETS_NS
    Asset::Asset(InternedString path, Asset **nested, uint32_t nestedCount)
        : mPath(path),
          mName(std::filesystem::path(path.c_str()).filename().string()),
          mNestedResources(nested),
          mNestedResourceCount(nestedCount) {
    }

    bool Asset::hasNested() const {
        return mNestedResources != nullptr;
    }

    uint32_t Asset::getNestedCount() const {
        return mNestedResourceCount;
    }

    UPtr<Asset> Asset::clone() const {
        VOX_CHECK_FMT(false, "{} Asset cannot be cloned", +type());
    }

    void Asset::cloneTo(void *ptr) const {
        VOX_CHECK_FMT(false, "{} Asset cannot be cloned", +type());
    }

    Asset::~Asset() = default;
NS_END
