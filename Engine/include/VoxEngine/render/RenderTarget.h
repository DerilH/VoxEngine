//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderResource.h"
#include "Types.h"
#include "VoxCore/math/Extent.h"

RENDER_NS
class RenderTarget : public RenderResource {
    Extent mExtent;

protected:
    RenderTarget(Extent extent) : mExtent(extent) {}
public:
    RenderTarget() = delete;

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual TextureRef getBackBuffer() = 0;
    virtual void resize(Extent extent) = 0;
    inline Extent getSize() const {return mExtent;}
};
NS_END
