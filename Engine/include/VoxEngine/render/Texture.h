//
// Created by deril on 2/25/26.
//

#pragma once
#include "VoxEngine/render/graph/TextureHandle.h"

RENDER_NS
    class Texture {
    protected:
        const TextureHandle mHandle;
        explicit Texture(TextureHandle handle) : mHandle(handle) {
        }
    public:
        TextureHandle getHandle() const { return mHandle; }

        NO_COPY_MOVE_DEFAULT(Texture);
    };
NS_END
