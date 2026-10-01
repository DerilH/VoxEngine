//
// Created by deril on 2/17/26.
//

#include "VoxEngine/Engine.h"
#include <VoxEngine/render/shaders/ShaderCompiler.h>

#include <utility>
#include <VoxEngine/render/RendererFactory.h>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/Scene.h>
#include <VoxEngine/scene/components/RenderableMeshComponent.h>

#include "VoxEngine/render/RenderCore.h"
#include "VoxEngine/resources/assets/ModelAsset.h"
#include "VoxEngine/resources/assets/ShaderAsset.h"
#include "VoxEngine/render/RenderBackend.h"
#include "VoxEngine/resources/assets/MeshAsset.h"

VOX_NS
    constexpr const char* SHADERS_SRC_PATH = "./resources/shaders";
    constexpr const char* SHADERS_BIN_PATH = "./resources/shadersCache";

    Engine::Engine(std::string mTitle, const Render::RenderAPI renderApi) : mTitle(std::move(mTitle)), mRenderApi(renderApi) {
        VOX_CHECK(mRenderApi == Render::VULKAN_API, "Unsupported execute api");
    }

    Engine::~Engine() {
    }

    void Engine::init() {
        mResourceManager = &Resources::ResourcesManager::Get();
        Resources::ResourcesManager::SetRoot("./resources");
        Resources::ResourcesManager::Get().loadAll();

        auto window = new Render::Windowing::Window(mTitle, 920, 480);
        mWindows.emplace(mTitle,window);

        auto renderBackend = Render::CreateRenderBackend(Render::RenderAPI::VULKAN_API);
        mRenderer = Render::RendererFactory::Create(renderBackend);
        mRenderer->init();

        mRenderer->addRenderTarget(mRenderer->getBackend()->createWindowTarget(window->getExtent(), window->getHandle()));
    }

    void Engine::run() {
        scene = new Scene::Scene("Test");
        auto obj = scene->createObject("TestTeapot");
        auto meshComp = Scene::RenderableMeshComponent(obj);
        obj->attachComponent(&meshComp);
        meshComp.setMeshProvider([this]() {
            const auto t = mResourceManager->get<Resources::ModelAsset>("teapot.fbx");
            return t->getNested<Resources::MeshAsset>(0);
        });

        meshComp.setVertexShader(mResourceManager->get<Resources::ShaderAsset>("shaders/baseShader.vert"));
        meshComp.setFragmentShader(mResourceManager->get<Resources::ShaderAsset>("shaders/baseShader.frag"));

        auto obj1 = scene->createObject("TestTeapot1");
        auto meshComp1 = Scene::RenderableMeshComponent(obj1);
        obj1->attachComponent(&meshComp1);
        meshComp1.setMeshProvider([this]() {
            const auto t = mResourceManager->get<Resources::ModelAsset>("teapot1.fbx");
            return t->getNested<Resources::MeshAsset>(0);
        });

        meshComp1.setVertexShader(mResourceManager->get<Resources::ShaderAsset>("shaders/baseShader.vert"));
        meshComp1.setFragmentShader(mResourceManager->get<Resources::ShaderAsset>("shaders/baseShader.frag"));

        while (!mWindows[mTitle]->shouldClose()) {
            const auto renderable = scene->getAllRenderables();
            mRenderer->render(renderable);
            Render::Windowing::Window::pollEvents();
        }
    }

    Render::Windowing::Window * Engine::getWindow(std::string name) const {
        const auto res = mWindows.find(name);
        return res != mWindows.end() ? res->second : nullptr;
    }

    Render::Renderer* Engine::getRenderer() const {
        return mRenderer;
    }

NS_END
