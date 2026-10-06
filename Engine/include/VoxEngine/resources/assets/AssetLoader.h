//
// Created by deril on 2/19/26.
//

#pragma once
#include "VoxCore/Define.h"
#include "Asset.h"
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/ArrayView.h>

RESOURCES_NS
    class AssetLoader {
public:
    AssetLoader() = default;
    NO_COPY_MOVE(AssetLoader)
    virtual ~AssetLoader() = default;
    virtual Ref<Asset> load(InternedString path, ArrayView<void> data) = 0;
};
NS_END
