//
// Created by deril on 2/19/26.
//

#include "RegularFileImporter.h"
#include "../RegularFileAsset.h"
#include "../../AssetManager.h"

ASSETS_NS
    Ref<Asset> RegularFileImporter::load(FILE *file) {
        auto data = readFileData(file);
        return new RegularFileAsset(*data);
    }
NS_END
