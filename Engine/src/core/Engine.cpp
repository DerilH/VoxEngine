//
// Created by deril on 2/17/26.
//

#include "VoxEngine/Engine.h"
#include <VoxEngine/render/shaders/ShaderCompiler.h>

#include <utility>
#include <VoxEngine/render/RendererFactory.h>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/Scene.h>
#include <VoxEngine/scene/components/MeshRendererComponent.h>

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

        auto window = new Render::Windowing::Window(mTitle, 1400, 900);
        mWindows.emplace(mTitle,window);

        auto renderBackend = Render::CreateRenderBackend(Render::RenderAPI::VULKAN_API);
        mRenderer = Render::RendererFactory::Create(renderBackend);
        mRenderer->init();

        mRenderer->addRenderTarget(mRenderer->getBackend()->createWindowTarget(window->getExtent(), window->getHandle()));
    }

    void Engine::run() {
        scene = new Scene::Scene("Test");

        auto mat = mResourceManager->get<Resources::MaterialAsset>("materials/test.vmat");

        auto obj = scene->createObject("TestTeapot");
        auto meshComp = Scene::MeshRendererComponent(obj);
        obj->attachComponent(&meshComp);
        meshComp.setMeshProvider([this]() {
            const auto t = mResourceManager->get<Resources::ModelAsset>("teapot.fbx");
            return t->getNested<Resources::MeshAsset>(0);
        });
        meshComp.setMaterial(mat);

        auto obj1 = scene->createObject("TestTeapot1");
        auto meshComp1 = Scene::MeshRendererComponent(obj1);
        obj1->attachComponent(&meshComp1);
        meshComp1.setMeshProvider([this]() {
            const auto t = mResourceManager->get<Resources::ModelAsset>("teapot1.fbx");
            return t->getNested<Resources::MeshAsset>(0);
        });
        meshComp1.setMaterial(mat);

        while (!mWindows[mTitle]->shouldClose()) {
            mResourceManager->processDirty();
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
