//
// Created by deril on 2/16/26.
//

#pragma once
#include <VoxEngine/render/FpsCounter.h>
#include <VoxEngine/render/windowing/Window.h>

#include <glm/glm.hpp>
#include <VoxEngine/Engine.h>

#include "AssetExplorer.h"

namespace Vox::Editor {
    class Gui : SingletonBase<Gui>{
        Render::Window* mWindow = nullptr;
        Engine* mEngine = nullptr;
        Render::Renderer* mRenderer = nullptr;
        bool mInitialized = false;
        Render::FpsCounter mFpsCounter;
        glm::vec3 mMeshRotation = glm::vec3(0, 0, 0);
        Ref<Scene::GameObject> mSelected = nullptr;
        AssetExplorer mExplorer;
    public:

        void init(Render::Window& window, Engine& engine);
        void render(Render::RenderContext cmd);
        void renderTreePanel();
        void renderSelectedOptions();
        void renderTransformOptions();
        void renderComponents();
    };
}
