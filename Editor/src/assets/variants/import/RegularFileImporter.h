//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetImporter.h"

ASSETS_NS
class RegularFileImporter : public AssetImporter {
    Asset *load(InternedString path, ArrayView<void> data) override;
};
NS_END
