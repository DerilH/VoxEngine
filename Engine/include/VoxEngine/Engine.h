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
        Ref<Render::Renderer> mRenderer = nullptr;
        Ref<Resources::ResourcesManager> mResourceManager = nullptr;

        bool mInitialized = false;
        bool mShouldClose = false;
        InternedString mTitle;
        Render::RenderAPI mRenderApi;

        NO_COPY_MOVE_DEFAULT(Engine)

    public:
        Ref<Scene::Scene> scene;

        explicit Engine(InternedString mTitle, Render::RenderAPI renderApi);

        ~Engine();

        void init();

        void run();

        Render::Renderer *getRenderer() const;

        void shutdown() {
            LOG_INFO("Shutting down engine");
            mShouldClose = true;
        }
    };

NS_END
