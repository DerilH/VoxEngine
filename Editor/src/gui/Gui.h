//
// Created by deril on 2/16/26.
//

#pragma once
#include <VoxEngine/render/FpsCounter.h>
#include <VoxEngine/render/windowing/Window.h>
#include <VoxEngine/Engine.h>
#include "../assets/AssetManager.h"
#include "screens/Screen.h"

namespace Vox::Editor {

    class Gui : SingletonBase<Gui>{
        inline static Assets::AssetManager *sAssetManager = nullptr;
        Render::Window* mWindow = nullptr;
        Ref<Engine> mEngine = nullptr;
        Render::Renderer* mRenderer = nullptr;
        bool mInitialized = false;
        Render::FpsCounter mFpsCounter;
        HashMap<InternedString, UPtr<Screen>> mScreens;
    public:
        static Assets::AssetManager *GetAssetManager() {
            return sAssetManager;
        }

        void init(Render::Window& window, Engine& engine, Assets::AssetManager &assetManager);

        void render(Render::RenderContext context);
        Extent getMousePosition() const;
    };
}
