#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/math/UUID.h>
#include <VoxCore/render/Enums.h>
#include "Asset.h"
#include "ClonableAsset.h"
#include "../JsonSerialization.h"

ASSETS_NS
    class MaterialAsset : public CloneableAsset<MaterialAsset>, Resources::Serializable<Resources::JsonSerialization> {
    protected:
        MaterialAsset();
        MaterialAsset *cloneImpl(void *ptr) const override;

    public:
        MaterialAsset(HashMap<Render::ShaderStage, UUID> &shaders, Render::PolygonMode polygonMode, Render::CullMode cullMode, Render::PrimitiveTopology topology, UUID albedo, bool depthTest = true, bool depthWrite = true, Render::CompareOp depthCompareOp = Render::CompareOp::LESS_OR_EQUAL);

        HashMap<Render::ShaderStage, UUID> shaders;
        Render::PolygonMode polygonMode;
        Render::CullMode cullMode;
        Render::PrimitiveTopology topology;
        bool depthTest = true;
        bool depthWrite = true;
        Render::CompareOp depthCompareOp = Render::CompareOp::LESS_OR_EQUAL;
        UUID albedo;

        ~MaterialAsset() override;

        void serialize(FILE *file) const override;

        static Expected<MaterialAsset*> deserialize(FILE *file);


        AssetType type() const override {
            return StaticType();
        }

        static AssetType StaticType() {
            return AssetType::MATERIAL;
        }
    };

NS_END

namespace Vox::Resources {
    using MaterialAsset = Vox::Editor::Assets::MaterialAsset;
}
