#include <shaderc/shaderc.hpp>
#include <VoxEngine/render/shaders/ShaderCompiler.h>
#include <spirv_cross.hpp>
#include <VoxCore/Pointers.h>
#include <VoxEngine/render/shaders/ShaderUniform.h>

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

    Vector<ShaderUniform> getUniforms(spirv_cross::Compiler& compiler, const spirv_cross::SmallVector<spirv_cross::Resource>& inUniforms, ShaderResourceType resType) {
        Vector<ShaderUniform> uniforms;
        for (const auto& u: inUniforms) {
            const spirv_cross::SPIRType& type = compiler.get_type(u.base_type_id);
            size_t size = 0;
            if (type.basetype == spirv_cross::SPIRType::Struct) {
                size = compiler.get_declared_struct_size(type);
            }
            uint32_t binding = compiler.get_decoration(u.id, spv::DecorationBinding);
            uint32_t set = compiler.get_decoration(u.id, spv::DecorationDescriptorSet);
            uint32_t location = compiler.get_decoration(u.id, spv::DecorationLocation);
            uint32_t arraySize = type.array.empty() ? 1 : type.array[0];
            InternedString name = compiler.get_name(u.id);
            uniforms.emplace_back(name, location, binding, set, size, arraySize, resType);
        }
        return uniforms;
    }

    CompiledShader ShaderCompiler::compile(const ShaderSrc& src) const {
        const shaderc::SpvCompilationResult result = mCompiler.CompileGlslToSpv(
                src.src,
                shaderc_glsl_infer_from_source,
                "",
                mCompilerOptions
        );

        if (result.GetCompilationStatus() == shaderc_compilation_status_invalid_stage) {
            VOX_CHECK(false, "Invalid stage in shader, maybe you forgot #pragma directive");
        }

        VOX_CHECK(result.GetCompilationStatus() == shaderc_compilation_status_success, result.GetErrorMessage());
        const std::vector bin(result.cbegin(), result.cend());

        spirv_cross::Compiler compiler(bin);
        auto model = compiler.get_execution_model();
        auto type = shaderTypeFrom(model);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();
        auto inputs = getAttributes(compiler, resources.stage_inputs, ShaderAttribute::Direction::IN);
        auto outputs = getAttributes(compiler, resources.stage_outputs, ShaderAttribute::Direction::OUT);

        Vector<ShaderUniform> uniforms;
        auto ubos = getUniforms(compiler, resources.uniform_buffers, ShaderResourceType::UNIFORM_BUFFER);
        uniforms.insert(uniforms.end(), ubos.begin(), ubos.end());

        auto samplers = getUniforms(compiler, resources.sampled_images, ShaderResourceType::COMBINED_IMAGE_SAMPLER);
        uniforms.insert(uniforms.end(), samplers.begin(), samplers.end());

        auto ssbos = getUniforms(compiler, resources.storage_buffers, ShaderResourceType::STORAGE_BUFFER);
        uniforms.insert(uniforms.end(), ssbos.begin(), ssbos.end());

        auto storageImages = getUniforms(compiler, resources.storage_images, ShaderResourceType::STORAGE_IMAGE);
        uniforms.insert(uniforms.end(), storageImages.begin(), storageImages.end());

        auto sepSamplers = getUniforms(compiler, resources.separate_samplers, ShaderResourceType::SAMPLER);
        uniforms.insert(uniforms.end(), sepSamplers.begin(), sepSamplers.end());

        auto sepImages = getUniforms(compiler, resources.separate_images, ShaderResourceType::SAMPLED_IMAGE);
        uniforms.insert(uniforms.end(), sepImages.begin(), sepImages.end());

        Vector<PushConstantRange> pushConstants;
        for (const auto& pc: resources.push_constant_buffers) {
            const spirv_cross::SPIRType& spvType = compiler.get_type(pc.base_type_id);
            size_t size = 0;
            if (spvType.basetype == spirv_cross::SPIRType::Struct) {
                size = compiler.get_declared_struct_size(spvType);
            }
            pushConstants.emplace_back(0, static_cast<uint32_t>(size), type);
        }

        return CompiledShader(type, std::move(inputs), std::move(outputs), std::move(uniforms), "", std::move(bin), std::move(pushConstants));
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
