#pragma once
#include <fstream>

namespace Vox::Render::Shader {
    class ShaderSrc {
    public:
        const ShaderType type;
        std::string src;
        std::string name;

        ShaderSrc(const ShaderType type, std::string src, std::string name) : type(type), src(std::move(src)), name(std::move(name))
        {}

        static ShaderSrc load(const std::filesystem::path& path) {
            const std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file " + path.filename().string());
            }

            std::stringstream ss;
            ss << file.rdbuf();
            return {shaderTypeFrom(path), ss.str(), path.filename().string()};
        }
    };
}