//
// Created by deril on 2/17/26.
//

#pragma once
#include <VoxCore/signal/Event.h>

#include "VoxEngine/render/Renderer.h"
#include "VoxEngine/render/vulkan/VulkanRenderTarget.h"
#include "VoxEngine/render/windowing/Window.h"
#include "VoxEngine/resources/ResourceManager.h"
#include "VoxEngine/render/RenderCore.h"

VOX_NS
    class Engine {
        Ref<Render::Renderer> mRenderer = nullptr;
        UPtr<Resources::ResourceManager> mResourceManager = nullptr;
        UPtr<Scene::GameObjectRegistry> mGameObjectRegistry = nullptr;

        bool mInitialized = false;
        bool mShouldClose = false;
        InternedString mTitle;
        Render::RenderAPI mRenderApi;

        NO_COPY_MOVE_DEFAULT(Engine)
    public:
        UPtr<Scene::Scene> scene;
        Event<Ref<Scene::Scene>> onSceneChanged;

        explicit Engine(InternedString mTitle, Render::RenderAPI renderApi);

        ~Engine();

        void init();

        void run();

        Render::Renderer *getRenderer() const {
            return mRenderer;
        }

        Ref<Resources::ResourceManager> getResourceManager() const {
            return mResourceManager.get();
        }

        Ref<Scene::GameObjectRegistry> getGameObjectsRegistry() const {
            return mGameObjectRegistry.get();
        }

        void shutdown() {
            LOG_INFO("Shutting down engine");
            mShouldClose = true;
        }
    };

NS_END
