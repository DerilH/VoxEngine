#include <utility>

#include "../../../../Engine/include/VoxEngine/resources/Material.h"

Vox::Resources::Material::Material(InternedString path, HashMap<Render::ShaderStage, InternedString> &shaders,
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

Vox::Resources::Material::~Material() = default;

Vox::Resources::Material * Vox::Resources::Material::cloneImpl(void *ptr) const {
    return new (ptr) Material(*this);
}
