//
// Created by deril on 2/19/26.
//

#include "../RegularFileLoader.h"
#include "../RegularFile.h"

ASSETS_NS
Asset *RegularFileLoader::load(InternedString path, ArrayView<void> data) {
    return new RegularFile(path, data);
}
NS_END
