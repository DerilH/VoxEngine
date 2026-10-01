//
// Created by deril on 2/17/26.
//

#pragma once
#include "VoxEngine/render/Renderer.h"
#include "VoxEngine/render/vulkan/VulkanRenderTarget.h"
#include "VoxEngine/render/windowing/Window.h"
#include "VoxEngine/resources/ResourcesManager.h"
#include "VoxEngine/render/RenderCore.h"

VOX_NS

class Engine {
    std::unordered_map<std::string, Render::Windowing::Window*> mWindows;
    Ref<Render::Renderer> mRenderer = nullptr;
    Ref<Resources::ResourcesManager> mResourceManager = nullptr;

    bool mInitialized = false;
    std::string mTitle;
    Render::RenderAPI mRenderApi;

    NO_COPY_MOVE_DEFAULT(Engine)
public:
    Ref<Scene::Scene> scene;
    explicit Engine(std::string mTitle, Render::RenderAPI renderApi);
    ~Engine();
    void init();
    void run();
    Render::Windowing::Window* getWindow(std::string name) const;
    Render::Renderer* getRenderer() const;
};
NS_END