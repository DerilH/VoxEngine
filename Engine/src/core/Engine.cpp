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
#include "VoxEngine/render/RenderBackend.h"
#include "VoxEngine/scene/components/CameraComponent.h"
#include "VoxEngine/scene/GameObjectRegistry.h"

VOX_NS
    Engine::Engine(std::string mTitle, const Render::RenderAPI renderApi)
        : mTitle(std::move(mTitle)),
          mRenderApi(renderApi),
          mResourceManager(makeUPtr<Resources::ResourceManager>()),
          mGameObjectRegistry(new Scene::GameObjectRegistry()) {
        VOX_CHECK(mRenderApi == Render::VULKAN_API, "Unsupported render api");
    }

    Engine::~Engine() = default;

    void Engine::init() {
        mRenderer = Render::RendererFactory::Create(Render::RenderAPI::VULKAN_API, mResourceManager.get());
        mRenderer->init();
    }

    void Engine::run() {
        scene = makeUPtr<Scene::Scene>("Test", mGameObjectRegistry.get());
        onSceneChanged.invoke(scene.get());

        const auto obj = scene->createObject("TestTeapot");
        auto meshComp = Scene::MeshRendererComponent(obj);
        obj->attachComponent(&meshComp);

        auto meshHandle = mResourceManager->find<Resources::MeshResource>("defaults/DefaultCube.fbx/Cube");
        if (meshHandle) {
            auto meshRes = mResourceManager->get(*meshHandle);
            if (meshRes) {
                meshComp.setMeshProvider([mesh = *meshRes]() {
                    return mesh;
                });
            }
        }

        auto matHandle = mResourceManager->find<Resources::MaterialResource>("materials/test.vmat");
        if (matHandle) {
            auto matRes = mResourceManager->get(*matHandle);
            if (matRes) {
                meshComp.setMaterial(*matRes);
            }
        }

        //TODO add separate camera for editor;
        auto camera = scene->createObject("DefaultCamera");
        camera->attachComponent(new Scene::CameraComponent(camera));
        scene->setActiveCamera(camera->findComponent<Scene::CameraComponent>());
        camera->transform->setPos(Vec3(0, 0, 10));
        while (!mShouldClose) {
            mRenderer->render(scene.get());
            Render::Window::pollEvents();
        }
    }

NS_END
