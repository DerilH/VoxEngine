#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/render/Enums.h>
#include "Asset.h"
#include "ClonableAsset.h"

RESOURCES_NS
    class MaterialAsset : public CloneableAsset<MaterialAsset> {
    protected:
        MaterialAsset *cloneImpl(void *ptr) const override;

    public:
        MaterialAsset(InternedString path, HashMap<Render::ShaderStage, InternedString> &shaders,
                      Render::PolygonMode polygonMode, Render::CullMode cullMode, Render::PrimitiveTopology topology,
                      bool depthTest = true, bool depthWrite = true, Render::CompareOp depthCompareOp = Render::CompareOp::LESS_OR_EQUAL);

        HashMap<Render::ShaderStage, InternedString> shaders;
        Render::PolygonMode polygonMode;
        Render::CullMode cullMode;
        Render::PrimitiveTopology topology;
        bool depthTest = true;
        bool depthWrite = true;
        Render::CompareOp depthCompareOp = Render::CompareOp::LESS_OR_EQUAL;
        InternedString albedo;

        ~MaterialAsset() override;

        AssetType type() const override {
            return StaticType();
        }

        static AssetType StaticType() {
            return AssetType::MATERIAL;
        }
    };

NS_END
