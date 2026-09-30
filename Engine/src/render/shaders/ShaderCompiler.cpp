#include <shaderc/shaderc.hpp>
#include <VoxEngine/render/shaders/ShaderCompiler.h>
#include <spirv_cross.hpp>

namespace Vox::Render::Shaders {
    std::vector<std::filesystem::path> ShaderCompiler::listShaders(const std::filesystem::path& path) {
        std::vector<std::filesystem::path> shaders;
        for (const auto& entry: std::filesystem::directory_iterator(path)) {
            if (!isValidType(entry.path())) {
                LOG_WARN("Unsupported shader extension: {}", entry.path().string());
                continue;
            }

            shaders.emplace_back(entry.path());
        }
        return shaders;
    }


    std::vector<CompiledShader> ShaderCompiler::compileShaders(const std::filesystem::path& path) const {
        return compileShaders(listShaders(path));
    }

    std::vector<CompiledShader> ShaderCompiler::compileShaders(const std::vector<std::filesystem::path>& paths) const {
        std::vector<CompiledShader> shaders;
        for (const auto& entry: paths) {
            try {
                ShaderSrc src = ShaderSrc::load(entry);
                CompiledShader compiled = compile(src);

                shaders.emplace_back(compiled);
            } catch (std::exception& e) {
                LOG_ERROR("Failed to compile file {}", entry.filename().string());
            }
        }
        return shaders;
    }

    ShaderDataType typeFrom(spirv_cross::SPIRType::BaseType type) {
        switch (type) {
            case spirv_cross::SPIRType::Float:
                return ShaderDataType::FLOAT;
            case spirv_cross::SPIRType::SByte:
                return ShaderDataType::INT8;
            case spirv_cross::SPIRType::UByte:
                return ShaderDataType::UINT8;
            case spirv_cross::SPIRType::Short:
                return ShaderDataType::INT16;
            case spirv_cross::SPIRType::UShort:
                return ShaderDataType::UINT16;
            case spirv_cross::SPIRType::Int:
                return ShaderDataType::INT32;
            case spirv_cross::SPIRType::UInt:
                return ShaderDataType::UINT32;
            case spirv_cross::SPIRType::AtomicCounter:
                return ShaderDataType::ATOMIC_COUNTER;
            case spirv_cross::SPIRType::Void:
                return ShaderDataType::VOID;
            default:
                return ShaderDataType::UNKNOWN;
        }
    }

    Vector<ShaderAttribute> getAttributes(spirv_cross::Compiler& compiler, spirv_cross::SmallVector<spirv_cross::Resource> attributes, ShaderAttribute::Direction direction) {
        Vector<ShaderAttribute> attributesVec;
        for (const auto& attrib: attributes) {
            std::string name = compiler.get_name(attrib.id);

            uint32_t location = compiler.get_decoration(attrib.id, spv::DecorationLocation);
            spirv_cross::SPIRType type = compiler.get_type(attrib.type_id);
            attributesVec.emplace_back(name, typeFrom(type.basetype), location, type.vecsize, direction);
        }
        return attributesVec;
    }

    Vector<ShaderUniform> getUniforms(spirv_cross::Compiler& compiler, spirv_cross::SmallVector<spirv_cross::Resource> inUniforms) {
        Vector<ShaderUniform> uniforms;
        for (auto& u: inUniforms) {
            const spirv_cross::SPIRType& type = compiler.get_type(u.base_type_id);
            size_t size = compiler.get_declared_struct_size(type);
            uint32_t binding = compiler.get_decoration(u.id, spv::DecorationBinding);
            uint32_t set = compiler.get_decoration(u.id, spv::DecorationDescriptorSet);
            uint32_t location = compiler.get_decoration(u.id, spv::DecorationLocation);
            uint32_t arraySize = type.array.empty() ? 0 : type.array[0];
            InternedString name = compiler.get_name(u.id);
//            Vector<ShaderUniform::Member> members;
//            for (uint32_t i = 0; i < type.member_types.size(); i++)
//            {
//                auto& memberType = compiler.get_type(type.member_types[i]);
//                std::string memberName = compiler.get_member_name(type.self, i);
//                size_t offset = compiler.type_struct_member_offset(type, i);
//                members.emplace_back({memberName, memberType.})
//            }
            uniforms.emplace_back(name, location, binding, set, size, arraySize);
        }
        return uniforms;
    }

    CompiledShader ShaderCompiler::compile(const ShaderSrc& src) const {
        const shaderc::SpvCompilationResult result = mCompiler.CompileGlslToSpv(
                src.src,
                vox2shaderc(src.type),
                src.name.c_str(),
                mCompilerOptions
        );

        VOX_CHECK(result.GetCompilationStatus() == shaderc_compilation_status_success, result.GetErrorMessage());
        const std::vector bin(result.cbegin(), result.cend());

        spirv_cross::Compiler compiler(bin);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        auto inputs = getAttributes(compiler, resources.stage_inputs, ShaderAttribute::Direction::IN);
        auto outputs = getAttributes(compiler, resources.stage_outputs, ShaderAttribute::Direction::OUT);
        auto uniforms = getUniforms(compiler, resources.uniform_buffers);
        return CompiledShader(src.type, inputs, outputs, uniforms, src.name, bin);
    }

    shaderc_shader_kind getShaderKind(const std::filesystem::path& path) {
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
