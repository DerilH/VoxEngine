//
// Created by deril on 2/16/26.
//

#pragma once
#include <VoxEngine/render/FpsCounter.h>
#include <VoxEngine/render/windowing/Window.h>
#include <VoxEngine/Engine.h>
#include "screens/Screen.h"

namespace Vox::Editor {

    class Gui : SingletonBase<Gui>{
        Render::Window* mWindow = nullptr;
        Ref<Engine> mEngine = nullptr;
        Render::Renderer* mRenderer = nullptr;
        bool mInitialized = false;
        Render::FpsCounter mFpsCounter;
        HashMap<InternedString, UPtr<Screen>> mScreens;
    public:

        void init(Render::Window& window, Engine& engine);

        void render(Render::RenderContext context);
    };
}
