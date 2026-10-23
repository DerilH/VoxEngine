//
// Created by deril on 2/19/26.
//

#include "RegularFileImporter.h"
#include "../RegularFileAsset.h"

ASSETS_NS
    Ref<Asset> RegularFileImporter::load(FILE *data) {
        return new RegularFileAsset(data);
    }
NS_END
