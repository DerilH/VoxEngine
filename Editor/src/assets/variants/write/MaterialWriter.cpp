//
// Created by deril on 10/5/26.
//

#include "MaterialWriter.h"
#include <yyjson.h>
#include <fstream>
#include <magic_enum/magic_enum.hpp>
#include <VoxCore/Assert.h>
#include "../MaterialAsset.h"

ASSETS_NS
    void MaterialWriter::writeToFile(FILE* file, Asset* asset) {
        static_cast<MaterialAsset *>(asset)->serialize(file);
        fclose(file);
        LOG_VERBOSE("Material saved to {}");
    }
NS_END
