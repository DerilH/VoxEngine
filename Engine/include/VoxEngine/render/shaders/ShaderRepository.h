#pragma once
#include <string>
#include <unordered_map>

#include "CompiledShader.h"

namespace Vox::Render::Shader {
    class ShaderRepository {
        std::unordered_map<std::string, CompiledShader> mShaders;
    public:
        ShaderRepository();
        ~ShaderRepository();
        void add(const CompiledShader &shader);

        std::optional<std::reference_wrapper<CompiledShader>> get(const std::string &name);
    };
}
