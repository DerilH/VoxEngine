//
// Created by deril on 2/16/26.
//

#pragma once
#include <VoxEngine/render/FpsCounter.h>
#include <VoxEngine/render/vulkan/FrameSync.h>
#include <VoxEngine/render/windowing/Window.h>

namespace Vox::Editor {
    class Gui {
        const Render::Windowing::Window& mWindow;
        bool mInitialized = false;
        Render::FpsCounter mFpsCounter;
    public:
        explicit Gui(const Render::Windowing::Window &m_window);

        void init();
        void render(Render::Vulkan::FrameSync& frame);
    };
}
