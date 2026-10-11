//
// Created by deril on 3/3/26.
//

#pragma once


#include <cstdint>
#include "VoxCore/Define.h"
#include "VoxCore/Pointers.h"
#include "VoxEngine/render/Types.h"

RENDER_NS

class GraphTexture {
    std::function<Ref<Texture>()> mTextureProvider = []() {
        return static_cast<Ref<Texture>>(nullptr);
    };
public:
    const uint32_t id;
    GraphTexture(uint32_t id) : id(id) {}

    inline TextureRef getExact() const {
        return mTextureProvider();
    }

    inline void setExact(const std::function<Ref<Texture>()> &provider) {
        mTextureProvider = provider;
    }

    NO_COPY_MOVE(GraphTexture)
};
NS_END
