#include "MaterialAsset.h"
#include <utility>
#include <VoxCore/math/UUID.h>

ASSETS_NS
    MaterialAsset::MaterialAsset(HashMap<Render::ShaderStage, UUID> &shaders,
                                 Render::PolygonMode polygonMode, Render::CullMode cullMode, Render::PrimitiveTopology topology,
                                 UUID albedo,
                                 bool depthTest, bool depthWrite, Render::CompareOp depthCompareOp)
        : CloneableAsset(nullptr, 0), shaders(std::move(shaders)),
          polygonMode(polygonMode),
          cullMode(cullMode),
          topology(topology),
          depthTest(depthTest),
          depthWrite(depthWrite),
          depthCompareOp(depthCompareOp),
          albedo(albedo) {
    }

    MaterialAsset::~MaterialAsset() = default;

    void MaterialAsset::serialize(FILE *file) const {
        SERIALIZE(file, FIELD(shaders), FIELD(polygonMode), FIELD(cullMode), FIELD(topology), FIELD(depthTest), FIELD(depthWrite), FIELD(depthCompareOp), FIELD(albedo));
    }

    Expected<MaterialAsset*> MaterialAsset::deserialize(FILE *file) {
        auto m = new MaterialAsset();
        DESERIALIZE(file, FIELD_OF(m, shaders), FIELD_OF(m, polygonMode), FIELD_OF(m, cullMode), FIELD_OF(m, topology), FIELD_OF(m, depthTest), FIELD_OF(m, depthWrite), FIELD_OF(m, depthCompareOp), FIELD_OF(m, albedo));
        return m;
    }


    MaterialAsset::MaterialAsset() : CloneableAsset(nullptr, 0) {
    }

    MaterialAsset *MaterialAsset::cloneImpl(void *ptr) const {
        return new(ptr) MaterialAsset(*this);
    }

NS_END
