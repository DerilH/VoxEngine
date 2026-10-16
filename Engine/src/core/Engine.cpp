//
// Created by deril on 2/17/26.
//

#include "VoxEngine/Engine.h"
#include <VoxEngine/render/shaders/ShaderCompiler.h>

#include <utility>
#include <VoxEngine/render/RendererFactory.h>
#include <VoxEngine/render/windowing/WindowManager.h>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/Scene.h>
#include <VoxEngine/scene/components/MeshRendererComponent.h>
#include <VoxEngine/scene/components/Transform.h>

#include "VoxEngine/render/RenderCore.h"
#include "VoxEngine/resources/assets/ModelAsset.h"
#include "VoxEngine/resources/assets/ShaderAsset.h"
#include "VoxEngine/render/RenderBackend.h"
#include "VoxEngine/resources/assets/MeshAsset.h"
#include "VoxEngine/scene/components/CameraComponent.h"

VOX_NS
    constexpr const char *SHADERS_SRC_PATH = "./resources/shaders";
    constexpr const char *SHADERS_BIN_PATH = "./resources/shadersCache";

    Engine::Engine(std::string mTitle, const Render::RenderAPI renderApi) : mTitle(std::move(mTitle)), mRenderApi(renderApi), mResourceManager(new Resources::ResourceManager()) {
        VOX_CHECK(mRenderApi == Render::VULKAN_API, "Unsupported execute api");
    }

    Engine::~Engine() = default;

    void Engine::init() {
        mResourceManager->loadAll();

        mRenderer = Render::RendererFactory::Create(Render::RenderAPI::VULKAN_API, mResourceManager);

        mRenderer->init();
    }

    void Engine::run() {
        scene = makeUPtr<Scene::Scene>("Test");
        onSceneChanged.invoke(scene.get());


        const auto obj = scene->createObject("TestTeapot");
        auto meshComp = Scene::MeshRendererComponent(obj);
        obj->attachComponent(&meshComp);
        const auto t = mResourceManager->get<Resources::ModelAsset>("defaults/DefaultCube.fbx");
        if (t) {
            meshComp.setMeshProvider([this, t]() {
                return (*t)->getNestedUnsafe<Resources::MeshAsset>(0);
            });
        }

        const auto mat = mResourceManager->get<Resources::MaterialAsset>("materials/test.vmat");
        if (mat) {
            meshComp.setMaterial(*mat);
        }


        //TODO add separate camera for editor;
        auto camera = scene->createObject("DefaultCamera");
        camera->attachComponent(new Scene::CameraComponent(camera));
        scene->setActiveCamera(camera->findComponent<Scene::CameraComponent>());
        camera->transform->setPos(Vec3(0, 0, 10));
        while (!mShouldClose) {
            mResourceManager->processDirty();
            mRenderer->render(scene.get());
            Render::Window::pollEvents();
        }
    }

NS_END
