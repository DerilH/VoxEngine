#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/Pointers.h"

RENDER_NS
    class TextureRenderTarget : public RenderTarget {
    protected:
        Ref<Texture> mTexture;
        Ref<Texture> mDepthTexture;

    public:
        TextureRenderTarget(const Ref<Texture> texture, const Ref<Texture> depthTexture = nullptr) : RenderTarget(texture->getExtent()), mTexture(texture), mDepthTexture(depthTexture) {
        }

        TextureRef getBackBuffer() override {
            return mTexture;
        }

        TextureRef getDepthBuffer() override {
            return mDepthTexture;
        }
    };

NS_END
