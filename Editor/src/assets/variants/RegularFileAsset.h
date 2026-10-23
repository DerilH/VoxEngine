//
// Created by deril on 2/19/26.
//

#pragma once
#include <VoxCore/containers/ArrayView.h>
#include "Asset.h"

ASSETS_NS
    class RegularFileAsset : public Asset {
        ArrayView<void> mData;
    public:
        RegularFileAsset(ArrayView<void> data);
        ~RegularFileAsset() override;

        ArrayView<void> getData() const {
            return mData;
        }

        AssetType type() const override {
            return StaticType();
        }

        static AssetType StaticType() {
            return AssetType::REGULAR;
        }
    };
NS_END

namespace Vox::Resources {
    using RegularFileAsset = Vox::Editor::Assets::RegularFileAsset;
}
