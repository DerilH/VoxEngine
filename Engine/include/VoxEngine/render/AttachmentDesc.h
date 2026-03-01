//
// Created by deril on 2/26/26.
//

#pragma once


#include "VoxEngine/render/vulkan/passes/PassTransition.h"
#include "VoxEngine/render/graph/TextureHandle.h"

RENDER_NS
struct AttachmentDesc {
public:
    PassTransition transition;
    TextureHandle textureHandle;
    AttachmentDesc(TextureHandle& handle, PassTransition& trans) : textureHandle(handle), transition(trans) {}

    bool operator==(const AttachmentDesc& other) const {
        return textureHandle.id == other.textureHandle.id;
    }
};
NS_END
namespace std {
    template<>
    struct hash<Vox::Render::AttachmentDesc> {
        std::size_t operator()(const Vox::Render::AttachmentDesc &p) const noexcept {
            return p.textureHandle.id;
        }
    };
}