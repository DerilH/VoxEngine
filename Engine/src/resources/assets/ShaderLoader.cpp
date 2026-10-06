//
// Created by deril on 2/20/26.
//

#include "VoxEngine/resources/assets/ShaderLoader.h"
#include "VoxEngine/resources/assets/ShaderAsset.h"
#include "VoxCore/containers/Containers.h"
#include <string>
RESOURCES_NS
    Vox::Render::Shaders::ShaderCompiler ShaderLoader::compiler;

Asset *ShaderLoader::load(std::string path, ArrayView<void> data) {
    Render::Shaders::ShaderSrc src = Render::Shaders::ShaderSrc(Render::Shaders::shaderTypeFrom(path), InternedString(static_cast<const char *>(data.pData), data.size()), path);
    Render::Shaders::CompiledShader shader = compiler.compile(src);
    return new ShaderAsset(path, shader);
}
NS_END
