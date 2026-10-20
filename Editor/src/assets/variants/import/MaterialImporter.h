//
// Created by deril on 2/19/26.
//

#pragma once

#include <VoxCore/Pointers.h>

#include "AssetImporter.h"
#include "../AssetLoader.h"

ASSETS_NS
class MaterialImporter : public AssetImporter {
    Ref<Asset> load(InternedString path, ArrayView<void> data) override;
};
NS_END
