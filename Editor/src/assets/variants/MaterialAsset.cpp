#include "MaterialAsset.h"
#include <utility>

ASSETS_NS
    MaterialAsset::MaterialAsset(InternedString path, HashMap<Render::ShaderStage, InternedString> &shaders,
                                 Render::PolygonMode polygonMode, Render::CullMode cullMode, Render::PrimitiveTopology topology,
                                 bool depthTest, bool depthWrite, Render::CompareOp depthCompareOp,
                                 InternedString texture)
        : CloneableAsset(path, nullptr, 0), shaders(std::move(shaders)),
          polygonMode(polygonMode),
          cullMode(cullMode),
          topology(topology),
          depthTest(depthTest),
          depthWrite(depthWrite),
          depthCompareOp(depthCompareOp),
          texture(texture),
          albedo(texture) {
    }

    MaterialAsset::~MaterialAsset() = default;

    MaterialAsset * MaterialAsset::cloneImpl(void *ptr) const {
        return new (ptr) MaterialAsset(*this);
    }
NS_END
