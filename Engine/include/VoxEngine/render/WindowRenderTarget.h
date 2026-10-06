//
// Created by deril on 3/5/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/Pointers.h"
#include "RenderTarget.h"
#include "VoxEngine/render/windowing/Window.h"

RENDER_NS
    class WindowRenderTarget : public RenderTarget {
    protected:
        Ref<Window> mWindow;

    public:
        WindowRenderTarget(Ref<Window> mWindow) : RenderTarget(mWindow->getExtent()), mWindow(mWindow) {
        }
    };

NS_END
