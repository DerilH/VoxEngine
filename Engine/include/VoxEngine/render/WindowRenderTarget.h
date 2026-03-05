//
// Created by deril on 3/5/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderTarget.h"

RENDER_NS
    class WindowRenderTarget : public RenderTarget {
        void* mNativeWindow;
    public:
        WindowRenderTarget(Extent extent, void* nativeWindow) : RenderTarget(extent), mNativeWindow(nativeWindow) {
        }
    };
NS_END

