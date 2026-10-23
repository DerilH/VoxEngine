//
// Created by deril on 2/20/26.
//

#include "ShaderImporter.h"
#include "../ShaderAsset.h"
#include <VoxEngine/render/shaders/ShaderUtil.h>
#include <VoxCore/containers/Containers.h>

ASSETS_NS
    Vox::Render::Shaders::ShaderCompiler ShaderImporter::compiler;

    Ref<Asset> ShaderImporter::load(FILE *data) {
        Render::Shaders::ShaderSrc src(
            InternedString(static_cast<const char *>(data.pData), data.size())
        );
        Render::Shaders::CompiledShader shader = compiler.compile(src);
        return new ShaderAsset(shader);
    }
NS_END
