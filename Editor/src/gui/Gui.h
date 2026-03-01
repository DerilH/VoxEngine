//
// Created by deril on 2/16/26.
//

#pragma once
#include <VoxEngine/render/FpsCounter.h>
#include <VoxEngine/render/vulkan/FrameSync.h>
#include <VoxEngine/render/windowing/Window.h>

namespace Vox::Editor {
    class Gui : SingletonBase<Gui>{
        Render::Windowing::Window* mWindow;
        bool mInitialized = false;
        Render::FpsCounter mFpsCounter;
    public:


        void init(Render::Windowing::Window& window);
        void render(const Render::Vulkan::CommandBuffer& cmd);
    };
}
