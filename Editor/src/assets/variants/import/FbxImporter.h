//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetImporter.h"

ASSETS_NS
class FbxImporter : public AssetImporter {
    Asset *load(std::string path, ArrayView<void> data) override;
};
NS_END
