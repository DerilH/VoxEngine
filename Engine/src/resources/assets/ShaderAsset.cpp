//
// Created by deril on 2/20/26.
//

#include "VoxEngine/resources/assets/ShaderAsset.h"
#include "VoxEngine/render/shaders/CompiledShader.h"

RESOURCES_NS
    ShaderAsset::ShaderAsset(std::string path, Render::Shaders::CompiledShader mCompiled) : Asset(std::move(path)), mCompiled(std::move(mCompiled)) {
    }

    const Render::Shaders::CompiledShader &ShaderAsset::getCompiled() const {
        return mCompiled;
    }

NS_END
