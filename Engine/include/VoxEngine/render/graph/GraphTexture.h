//
// Created by deril on 3/3/26.
//

#pragma once


#include <cstdint>
#include <VoxCore/render/Enums.h>

#include "VoxCore/Define.h"
#include "VoxCore/Pointers.h"
#include "VoxEngine/render/Types.h"

RENDER_NS

class GraphTexture {
    std::function<Ref<Texture>()> mTextureProvider = []() {
        return static_cast<Ref<Texture>>(nullptr);
    };
    Format mFormat;
public:
    const uint32_t id;
    GraphTexture(uint32_t id, Format format) : mFormat(format), id(id) {}

    TextureRef getExact() const {
        return mTextureProvider();
    }

    inline void setExact(const std::function<Ref<Texture>()> &provider) {
        mTextureProvider = provider;
    }

    Format getFormat() const {
        return mFormat;
    }

    NO_COPY_MOVE(GraphTexture)
};
NS_END
