//
// Created by deril on 2/20/26.
//

#include "ShaderAsset.h"
#include <VoxEngine/render/shaders/CompiledShader.h>

ASSETS_NS
    ShaderAsset::ShaderAsset(std::string path, const Render::Shaders::CompiledShader compiled)
        : Asset(InternedString(path)), mCompiled(compiled) {
    }

    const Render::Shaders::CompiledShader &ShaderAsset::getCompiled() const {
        return mCompiled;
    }
NS_END
