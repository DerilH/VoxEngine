#pragma once
#include <fstream>
#include <utility>
#include <vector>
#include "ShaderUtil.h"
#include "ShaderAttribute.h"
#include "ShaderUniform.h"

SHADERS_NS
    struct CompiledShader {
    public:
        const ShaderStage stage;
        const Vector<ShaderAttribute> inputs;
        const Vector<ShaderAttribute> outputs;
        const Vector<ShaderUniform> uniforms;

        const std::vector<uint32_t> bin;
        const std::string name;
        explicit CompiledShader(const ShaderStage stage, Vector<ShaderAttribute> inputs, Vector<ShaderAttribute> outputs, Vector<ShaderUniform> uniforms, std::string name, std::vector<uint32_t> bin) : inputs(std::move(inputs)), outputs(std::move(outputs)), uniforms(std::move(uniforms)), stage(stage), bin(std::move(bin)), name(std::move(name)) {
        }

        void save(const std::filesystem::path &path) const {
            std::ofstream file(path);
            VOX_CHECK(file.is_open(), "Cannot save shader code")

            file.write(reinterpret_cast<const std::ostream::char_type *>(bin.data()), bin.size() * sizeof(std::ostream::char_type));
            file.flush();
            file.close();
        }
    };
NS_END
