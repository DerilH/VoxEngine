//
// Created by deril on 2/19/26.
//

#include "VoxEngine/resources/assets/ModelAsset.h"

RESOURCES_NS
    ModelAsset::ModelAsset(const std::string &path, Asset **nested, uint32_t nestedCount) : Asset(path, nested, nestedCount) {}

    AssetType ModelAsset::type() {
        return AssetType::MODEL;
    }
NS_END
