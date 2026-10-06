#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/render/Enums.h>
#include "Asset.h"
#include "ClonableAsset.h"

RESOURCES_NS
    class MaterialAsset : public CloneableAsset<MaterialAsset> {
    public:
        MaterialAsset(InternedString path, HashMap<Render::ShaderStage, InternedString> &shaders, Render::PolygonMode polygonMode, Render::CullMode cullMode, Render::PrimitiveTopology topology);

        HashMap<Render::ShaderStage, InternedString> shaders;
        Render::PolygonMode polygonMode;
        Render::CullMode cullMode;
        Render::PrimitiveTopology topology;

        ~MaterialAsset() override;

        MaterialAsset *cloneImpl(void *ptr) const override;

        AssetType type() const override {
            return StaticType();
        }

        static AssetType StaticType() {
            return AssetType::MATERIAL;
        }
    };
NS_END
