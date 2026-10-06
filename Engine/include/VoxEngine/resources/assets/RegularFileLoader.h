//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetLoader.h"

RESOURCES_NS
class RegularFileLoader : public AssetLoader {
    Asset *load(InternedString path, ArrayView<void> data) override;
};
NS_END
