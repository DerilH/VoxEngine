//
// Created by deril on 2/19/26.
//

#pragma once

#include <VoxCore/containers/ArrayView.h>

#include "Asset.h"

RESOURCES_NS
    class TextureAsset : public Asset {
        ArrayView<void> mPixels;
    public:
        TextureAsset(InternedString path, ArrayView<void> pixels) : Asset(path, nullptr), mPixels(pixels) {
        }

        ArrayView<void> getPixels() const {
            return mPixels;
        }

        AssetType type() const override {
            return StaticType();
        }

        static AssetType StaticType() {
            return AssetType::MODEL;
        }
    };

NS_END
