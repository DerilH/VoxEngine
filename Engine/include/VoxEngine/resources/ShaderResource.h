//
// Created by deril on 2/20/26.
//

#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Assert.h>
#include <VoxCore/containers/Containers.h>
#include <VoxEngine/render/shaders/CompiledShader.h>
#include "ResourceType.h"
#include "ResourceHandle.h"

RESOURCES_NS
    class ShaderResource {
        Render::Shaders::CompiledShader mCompiled;

    public:
        ShaderResource()
            : mCompiled(Render::ShaderStage::VERTEX, {}, {}, {}, "", {}) {}

        ShaderResource(Render::Shaders::CompiledShader compiled)
            :  mCompiled(std::move(compiled)) {}

        Expected<void> validate() const {
            VOX_EXPECT(!mCompiled.bin.empty(), "ShaderResource compiled bytecode is empty");
            return {};
        }

        const Render::Shaders::CompiledShader &getCompiled() const noexcept { return mCompiled; }
        Render::ShaderStage getStage() const noexcept { return mCompiled.stage; }
    };

    template<>
    struct ResourceTraits<ShaderResource> {
        static constexpr ResourceType Type = ResourceType::SHADER;
        static constexpr const char *Name = "Shader";
    };

    using ShaderHandle = ResourceHandle<ShaderResource>;
NS_END
