//
// Created by deril on 2/17/26.
//

#include "VoxEngine/Engine.h"
#include <VoxEngine/render/shaders/ShaderCompiler.h>

#include <utility>
#include <VoxEngine/render/RendererFactory.h>
#include <VoxEngine/render/vulkan/Surface.h>
#include <VoxEngine/render/vulkan/VulkanState.h>
#include <VoxEngine/render/vulkan/VulkanWindowRenderTarget.h>
#include "VoxEngine/render/vulkan/VulkanRenderer.h"

VOX_NS
    constexpr const char* SHADERS_SRC_PATH = "./resources/shaders";
    constexpr const char* SHADERS_BIN_PATH = "./resources/shadersCache";

    Engine::Engine(std::string mTitle, const Render::RenderAPI renderApi) : mTitle(std::move(mTitle)), mRenderApi(renderApi) {
        VOX_CHECK(mRenderApi == Render::VULKAN_API, "Unsupported render api");
    }

    Engine::~Engine() {
    }

    void Engine::init() {
        mResourceManager = &Vox::Resources::ResourcesManager::Get();
        Vox::Resources::ResourcesManager::SetRoot("./resources");
        Vox::Resources::ResourcesManager::Get().loadAll();

        auto window = new Render::Windowing::Window(mTitle, 920, 480);
        mRenderer = Render::RendererFactory::Create(mShaderRepository, Render::VULKAN_API);
        mRenderer->init();

        auto s1 = Render::Vulkan::Surface::Create(Render::Vulkan::VulkanState::Get()->instance, *window);
        s1->setDevice(Render::Vulkan::VulkanState::Get()->device);

        auto s = mResourceManager->get<Resources::ModelAsset>("mesh.fbx");
        ((Render::Vulkan::VulkanRenderer*)mRenderer)->registerMesh(s);

        mRenderer->addRenderTarget(reinterpret_cast<Render::RenderTarget *>(new Render::Vulkan::VulkanWindowRenderTarget(*s1)));
    }

    void Engine::run() {
        mRenderer->renderLoop();
    }


    Render::Windowing::Window * Engine::getWindow(std::string name) const {
        const auto res = mWindows.find(name);
        return res != mWindows.end() ? res->second : nullptr;
    }

NS_END
