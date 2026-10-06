//
// Created by deril on 3/5/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/Pointers.h"
#include "RenderTarget.h"
#include "Texture.h"

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
        int32_t beginFrame() override {}

        void endFrame() override {}

        void resize(Extent extent) override {}
        ~TextureRenderTarget() override;
    };

NS_END
