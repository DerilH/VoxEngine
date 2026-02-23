//
// Created by deril on 2/17/26.
//

#pragma once
#include "VoxEngine/render/Renderer.h"
#include "VoxEngine/render/vulkan/VulkanRenderTarget.h"
#include "VoxEngine/render/windowing/Window.h"
#include "VoxEngine/resources/ResourcesManager.h"

VOX_NS
class Engine {
    std::unordered_map<std::string, Render::Windowing::Window*> mWindows;
    Render::Renderer* mRenderer;
    Render::Shader::ShaderRepository mShaderRepository;
    Vox::Resources::ResourcesManager* mResourceManager;

    bool mInitialized = false;
    std::string mTitle;
    Render::RenderAPI mRenderApi;

    NON_COPYABLE_NON_MOVABLE(Engine)
public:

    explicit Engine(std::string mTitle, Render::RenderAPI renderApi);
    ~Engine();
    void init();
    void run();
    void compileShaders();
    Render::Windowing::Window* getWindow(std::string name) const;
};
NS_END