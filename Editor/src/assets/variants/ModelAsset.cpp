//
// Created by deril on 2/19/26.
//

#include "ModelAsset.h"

ASSETS_NS
    ModelAsset::ModelAsset(InternedString path, Asset **nested, uint32_t nestedCount)
        : Asset(path, nested, nestedCount) {}
NS_END
