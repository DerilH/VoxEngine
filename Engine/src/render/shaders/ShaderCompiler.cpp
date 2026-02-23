#include <shaderc/shaderc.hpp>
#include <VoxEngine/render/shaders/ShaderCompiler.h>

namespace Vox::Render::Shader {
    std::vector<std::filesystem::path> ShaderCompiler::listShaders(const std::filesystem::path &path) {
        std::vector<std::filesystem::path> shaders;
        for (const auto &entry: std::filesystem::directory_iterator(path)) {
            if (!isValidType(entry.path())) {
                LOG_WARN("Unsupported shader extension: {}", entry.path().string());
                continue;
            }

            shaders.emplace_back(entry.path());
        }
        return shaders;
    }


    std::vector<CompiledShader> ShaderCompiler::compileShaders(const std::filesystem::path &path) const {
        return compileShaders(listShaders(path));
    }

    std::vector<CompiledShader> ShaderCompiler::compileShaders(const std::vector<std::filesystem::path> &paths) const {
        std::vector<CompiledShader> shaders;
        for (const auto &entry: paths) {
            try {
                ShaderSrc src = ShaderSrc::load(entry);
                CompiledShader compiled = compile(src);

                shaders.emplace_back(compiled);
            } catch (std::exception &e) {
                LOG_ERROR("Failed to compile file {}", entry.filename().string());
            }
        }
        return shaders;
    }

    CompiledShader ShaderCompiler::compile(const ShaderSrc &src) const {
        const shaderc::SpvCompilationResult result = mCompiler.CompileGlslToSpv(
            src.src,
            vox2shaderc(src.type),
            src.name.c_str(),
            mCompilerOptions
        );

        VOX_CHECK(result.GetCompilationStatus() == shaderc_compilation_status_success, result.GetErrorMessage());
        const std::vector bin(result.cbegin(), result.cend());
        return CompiledShader(src.type, src.name, bin);
    }

    shaderc_shader_kind getShaderKind(const std::filesystem::path &path) {
        const auto ext = path.extension().string();

        if (ext == ".vert") return shaderc_vertex_shader;
        if (ext == ".frag") return shaderc_fragment_shader;
        if (ext == ".comp") return shaderc_compute_shader;
        if (ext == ".geom") return shaderc_geometry_shader;
        if (ext == ".tesc") return shaderc_tess_control_shader;
        if (ext == ".tese") return shaderc_tess_evaluation_shader;
        throw std::runtime_error("Unknown shader type");
    }
}
