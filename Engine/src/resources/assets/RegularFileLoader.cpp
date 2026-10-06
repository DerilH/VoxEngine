//
// Created by deril on 2/19/26.
//

#include "VoxEngine/resources/assets/RegularFileLoader.h"
#include "VoxEngine/resources/assets/RegularFile.h"

RESOURCES_NS
Asset *RegularFileLoader::load(std::string path, ArrayView<void> data) {
    return new RegularFile(path, data);
}
NS_END
