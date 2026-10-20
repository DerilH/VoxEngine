//
// Created by deril on 2/19/26.
//

#pragma once
#include "VoxCore/Define.h"
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/ArrayView.h>

#include "../Asset.h"

ASSETS_NS
    class AssetImporter {
    public:
        AssetImporter() = default;

        NO_COPY_MOVE(AssetImporter)

        virtual ~AssetImporter() = default;

        virtual Ref<Asset> load(InternedString path, ArrayView<void> data) = 0;
        virtual AssetType type() = 0;
    };

NS_END
