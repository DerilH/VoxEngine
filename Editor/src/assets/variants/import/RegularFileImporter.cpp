//
// Created by deril on 2/19/26.
//

#include "RegularFileImporter.h"
#include "../RegularFileAsset.h"

ASSETS_NS
    Ref<Asset> RegularFileImporter::load(InternedString path, ArrayView<void> data) {
        return new RegularFileAsset(path, data);
    }
NS_END
