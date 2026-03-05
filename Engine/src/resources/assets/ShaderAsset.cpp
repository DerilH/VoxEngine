//
// Created by deril on 2/20/26.
//

#include "VoxEngine/resources/assets/ShaderAsset.h"

RESOURCES_NS
    ShaderAsset::ShaderAsset(std::string path, const ArrayView<uint32_t> *mCompiled) : Asset(std::move(path)), mCompiled(mCompiled) {}

    const ArrayView<uint32_t>& ShaderAsset::getCompiled() const {
        return *mCompiled;
    }

    AssetType ShaderAsset::type() {
        return AssetType::SHADER;
    }
NS_END


