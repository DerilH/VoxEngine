#pragma once
#include <filesystem>
#include <vector>
#include <shaderc/shaderc.hpp>
#include "CompiledShader.h"
#include "ShaderSrc.h"

namespace Vox::Render::Shader {
    class ShaderCompiler {
    public:
        static std::vector<std::filesystem::path> listShaders(const std::filesystem::path& path);
        std::vector<CompiledShader> compileShaders(const std::filesystem::path &path) const;
        std::vector<CompiledShader> compileShaders(const std::vector<std::filesystem::path> &paths) const;
        CompiledShader compile(const ShaderSrc &src) const;

    private:
        shaderc::Compiler mCompiler;
        shaderc::CompileOptions mCompilerOptions;
    };
}
