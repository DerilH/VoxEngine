//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetLoader.h"

RESOURCES_NS
class FbxLoader : public AssetLoader {
    Asset *load(std::string path, ArrayView<void> data) override;
};
NS_END
