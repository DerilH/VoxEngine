//
// Created by deril on 2/20/26.
//

#include "ShaderImporter.h"
#include "../ShaderAsset.h"
#include <VoxEngine/render/shaders/ShaderUtil.h>
#include <VoxCore/containers/Containers.h>

ASSETS_NS
    Vox::Render::Shaders::ShaderCompiler ShaderImporter::compiler;

    Ref<Asset> ShaderImporter::load(InternedString path, ArrayView<void> data) {
        Render::Shaders::ShaderSrc src(
            Render::Shaders::shaderTypeFrom(path),
            InternedString(static_cast<const char *>(data.pData), data.size()),
            path.c_str()
        );
        Render::Shaders::CompiledShader shader = compiler.compile(src);
        return new ShaderAsset(path.c_str(), shader);
    }
NS_END
