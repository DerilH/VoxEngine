#pragma once
#include <fstream>
#include <utility>
#include <vector>
#include "ShaderType.h"

namespace Vox::Render::Shader {
    class CompiledShader {
    public:
        const ShaderType type;
        const std::vector<uint32_t> bin;
        const std::string name;
        explicit CompiledShader(const ShaderType type, std::string name, std::vector<uint32_t> bin) : type(type), bin(std::move(bin)), name(std::move(name)) {
        }

        void save(const std::filesystem::path &path) const {
            std::ofstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot save shader code");
            }

            file.write(reinterpret_cast<const std::ostream::char_type *>(bin.data()), bin.size() * sizeof(std::ostream::char_type));
            file.flush();
            file.close();
        }
    };
}
