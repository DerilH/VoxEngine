//
// Created by deril on 3/3/26.
//

#pragma once


#include "VoxCore/Define.h"
#include "VoxEngine/render/Types.h"

RENDER_NS
class GraphTexture {
    TextureRef mTexture;
public:
    const uint32_t id;
    GraphTexture(uint32_t id) : id(id) {}

    inline TextureRef getExact() const {
        return mTexture;
    }

    inline void setExact(TextureRef texture) {
        mTexture = texture;
    }

    NO_COPY_MOVE(GraphTexture)
};
NS_END
