//
// Created by deril on 2/19/26.
//

#pragma once

#include <VoxCore/Pointers.h>

#include "AssetLoader.h"

RESOURCES_NS
class MaterialLoader : public AssetLoader {
    Ref<Asset> load(InternedString path, ArrayView<void> data) override;
};
NS_END
