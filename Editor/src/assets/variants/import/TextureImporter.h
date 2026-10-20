//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetImporter.h"

ASSETS_NS
class TextureImporter : public AssetImporter {
    Asset *load(std::string path, ArrayView<void> data) override;
};
NS_END
