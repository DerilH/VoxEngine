#include <utility>

#include "VoxEngine/resources/assets/MaterialAsset.h"

Vox::Resources::MaterialAsset::MaterialAsset(InternedString path, HashMap<Render::ShaderStage, InternedString> &shaders, Render::PolygonMode polygonMode, Render::CullMode cullMode, Render::PrimitiveTopology topology) : CloneableAsset(path, nullptr, 0), shaders(std::move(shaders)),
                                                                                                                                                                                                                                                            polygonMode(polygonMode),
                                                                                                                                                                                                                                                            cullMode(cullMode),
                                                                                                                                                                                                                                                            topology(topology) {
}

Vox::Resources::MaterialAsset::~MaterialAsset() = default;

Vox::Resources::MaterialAsset * Vox::Resources::MaterialAsset::cloneImpl(void *ptr) const {
    return new (ptr) MaterialAsset(*this);
}

