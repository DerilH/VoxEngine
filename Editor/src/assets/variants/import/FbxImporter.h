//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetImporter.h"

ASSETS_NS
    class FbxImporter : public AssetImporter {
    public:
        Ref<Asset> load(FILE *data) override;
        AssetType type() override {
            return AssetType::MODEL;
        }
    };
NS_END
