//
// Created by deril on 2/17/26.
//

#pragma once

#include "VoxEngine/render/Enums.h"
#include "VoxCore/math/Extent.h"
#include "Texture.h"
#include "VoxCore/containers/Buffer.h"

RENDER_NS
class RenderTarget {
protected:
    Buffer<Texture*> mTextures;
    Extent mExtent;
    Format mFormat;

    ~RenderTarget() = default;

public:
    Extent getExtent() const {return mExtent;}
    Format getFormat() const {return mFormat;};
    virtual Texture* getTexture() = 0;
};
NS_END