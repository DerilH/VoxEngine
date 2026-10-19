//
// Created by deril on 2/19/26.
//

#pragma once

#include <VoxCore/containers/ArrayView.h>
#include <VoxCore/math/Extent.h>

#include "Asset.h"

RESOURCES_NS
    class TextureAsset : public Asset {
        ArrayView<void> mPixels;
        Extent mExtent;
    public:
        TextureAsset(InternedString path, ArrayView<void> pixels, Extent extent) : Asset(path, nullptr), mPixels(pixels), mExtent(extent) {
        }

        ArrayView<void> getPixels() const {
            return mPixels;
        }

        Extent getExtent() const {
            return mExtent;
        }

        AssetType type() const override {
            return StaticType();
        }

        static AssetType StaticType() {
            return AssetType::TEXTURE;
        }
    };

NS_END
