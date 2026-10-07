#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/Pointers.h"

RENDER_NS
    class TextureRenderTarget : public RenderTarget {
    protected:
        Ref<Texture> mTexture;

    public:
        TextureRenderTarget(const Ref<Texture> texture) : RenderTarget(texture->getExtent()), mTexture(texture) {
        }

        TextureRef getBackBuffer() override {
            return mTexture;
        }
    };

NS_END
