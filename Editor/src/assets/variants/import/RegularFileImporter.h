//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetImporter.h"

ASSETS_NS
    class RegularFileImporter : public AssetImporter {
    public:
        Ref<Asset> load(FILE *data) override;
        AssetType type() override {
            return AssetType::REGULAR;
        }
    };
NS_END
