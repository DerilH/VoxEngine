//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetImporter.h"

ASSETS_NS
    class TextureImporter : public AssetImporter {
    public:
        Ref<Asset> load(FILE *data) override;
        AssetType type() override {
            return AssetType::TEXTURE;
        }
    };
NS_END
