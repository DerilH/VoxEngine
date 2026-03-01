//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/graph/TextureHandle.h"

RENDER_NS
    bool TextureHandle::operator==(const TextureHandle &other) const {
        return id == other.id && generation == other.generation;
    }

    TextureHandle::TextureHandle(const uint32_t id, const uint32_t generation) : id(id), generation(generation) {

    }
NS_END
