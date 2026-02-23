//
// Created by deril on 2/19/26.
//

#pragma once

#include "Asset.h"

RESOURCES_NS
    class ModelAsset : public Asset {
        public:
        ModelAsset(const std::string &path, Asset **nested, uint32_t nestedCount);

        AssetType type() override;
    };
NS_END
