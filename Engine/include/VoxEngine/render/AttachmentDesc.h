//
// Created by deril on 2/26/26.
//

#pragma once


#include "VoxEngine/render/passes/PassTransition.h"
#include "VoxEngine/render/graph/GraphTexture.h"
#include "Types.h"

RENDER_NS
struct AttachmentDesc {
    GraphTextureRef texture;
    PassTransition transition;
    AttachmentDesc(GraphTextureRef handle, PassTransition trans) : texture(handle), transition(trans) {

    }

    bool operator==(const AttachmentDesc& other) const {
        return texture == other.texture && transition == other.transition;
    }
};
NS_END

namespace std {
    template<>
    struct hash<Vox::Render::AttachmentDesc> {
        size_t operator()(const Vox::Render::AttachmentDesc& desc) const noexcept {
            size_t h1 = hash<Vox::Render::GraphTextureRef>{}(desc.texture);
            size_t h2 = hash<int>{}(static_cast<int>(desc.transition));
            return h1 ^ (h2 << 1);
        }
    };
}
