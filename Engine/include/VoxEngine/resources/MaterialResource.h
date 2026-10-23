//
// Created by deril on 2/20/26.
//

#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Assert.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/render/Enums.h>
#include "ResourceType.h"
#include "ResourceHandle.h"
#include "ShaderResource.h"
#include "TextureResource.h"

RESOURCES_NS
    class MaterialResource {
        HashMap<Render::ShaderStage, ShaderHandle> mShaders;
        TextureHandle mTexture;

        Render::PolygonMode mPolygonMode = Render::PolygonMode::FILL;
        Render::CullMode mCullMode = Render::CullMode::BACK;
        Render::PrimitiveTopology mTopology = Render::PrimitiveTopology::TRIANGLE_LIST;
        bool mDepthTest = true;
        bool mDepthWrite = true;
        Render::CompareOp mDepthCompareOp = Render::CompareOp::LESS_OR_EQUAL;

    public:
        MaterialResource() = default;

        MaterialResource(HashMap<Render::ShaderStage, ShaderHandle> shaders,
                         Render::PolygonMode polygonMode,
                         Render::CullMode cullMode,
                         Render::PrimitiveTopology topology,
                         bool depthTest = true,
                         bool depthWrite = true,
                         Render::CompareOp depthCompareOp = Render::CompareOp::LESS_OR_EQUAL,
                         TextureHandle texture = {})
            :
              mShaders(std::move(shaders)),
              mTexture(texture),
              mPolygonMode(polygonMode),
              mCullMode(cullMode),
              mTopology(topology),
              mDepthTest(depthTest),
              mDepthWrite(depthWrite),
              mDepthCompareOp(depthCompareOp) {}

        Expected<void> validate() const {
            VOX_EXPECT(!mShaders.empty(), "MaterialResource has no shaders");
            VOX_EXPECT(mShaders.contains(Render::ShaderStage::VERTEX), "MaterialResource missing vertex shader");
            VOX_EXPECT(mShaders.contains(Render::ShaderStage::FRAGMENT), "MaterialResource missing fragment shader");
            for (const auto &[stage, handle] : mShaders) {
                VOX_EXPECT(handle.isValid(), "MaterialResource shader handle for stage {} is invalid", +stage);
            }
            return {};
        }

        const HashMap<Render::ShaderStage, ShaderHandle> &getShaders() const noexcept { return mShaders; }
        bool hasShader(Render::ShaderStage stage) const noexcept { return mShaders.contains(stage); }
        ShaderHandle getShader(Render::ShaderStage stage) const {
            auto it = mShaders.find(stage);
            return it != mShaders.end() ? it->second : ShaderHandle{};
        }
        void setShader(Render::ShaderStage stage, ShaderHandle handle) noexcept {
            mShaders[stage] = handle;
        }

        TextureHandle getTexture() const noexcept { return mTexture; }
        void setTexture(TextureHandle tex) noexcept { mTexture = tex; }

        Render::PolygonMode getPolygonMode() const noexcept { return mPolygonMode; }
        Render::CullMode getCullMode() const noexcept { return mCullMode; }
        Render::PrimitiveTopology getTopology() const noexcept { return mTopology; }
        bool isDepthTest() const noexcept { return mDepthTest; }
        bool isDepthWrite() const noexcept { return mDepthWrite; }
        Render::CompareOp getDepthCompareOp() const noexcept { return mDepthCompareOp; }
    };

    template<>
    struct ResourceTraits<MaterialResource> {
        static constexpr ResourceType Type = ResourceType::MATERIAL;
        static constexpr const char *Name = "Material";
    };

    using MaterialHandle = ResourceHandle<MaterialResource>;
NS_END
