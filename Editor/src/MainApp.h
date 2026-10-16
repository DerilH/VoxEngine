#pragma once

#include "VoxEngine/Engine.h"
#include <VoxEngine/platform/LinuxPlatform.h>
#include <VoxEngine/render/shaders/ShaderCompiler.h>
#include <VoxEngine/render/windowing/WindowManager.h>

#include <VoxEngine/render/vulkan/VulkanState.h>
#include <CLI/CLI.hpp>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/components/Transform.h>

#include "gui/Gui.h"

class MainApp {
    Vox::Engine *mEngine = nullptr;
    Vox::Editor::Gui *gui = nullptr;

public:
    void run(int argc, char **argv) {
        CLI::App app{"VoxEngine"};

        std::vector<std::filesystem::path> customSandboxPaths;
        bool enableSandbox = false;
        app.add_flag("--enable-sandbox", enableSandbox, "Enables sandbox for protection from unwanted filesystem changes");
        app.add_option("--sandbox-paths", customSandboxPaths,"Custom directories allowed in sandbox mode")->expected(1, -1);
        std::string dir = "default";
        app.add_option("--resources", dir, "Set app resources directory")->required();
        app.parse(argc, argv);

        Vox::Render::WindowManager::initBackend();

        mEngine = new Vox::Engine("Vox", Vox::Render::VULKAN_API);
        mEngine->getResourceManager()->setRoot(dir);
        mEngine->init();

        auto window = Vox::Render::WindowManager::Get().createWindow("Test", {1280, 720}, 0, true);
        Vox::Render::WindowManager::Get().setMainWindow(window);
        auto windowTarget = mEngine->getRenderer()->getBackend()->createWindowTarget(window);
        mEngine->getRenderer()->addViewportTarget("Scene", mEngine->getRenderer()->getBackend()->createTextureTarget(windowTarget->getSize(), Vox::Render::Format::BGRA8));
        mEngine->getRenderer()->addPresentTarget("MainWindow", windowTarget);
        mEngine->getRenderer()->createGraph();

        window->onKeyPressed += [this](auto w, auto key, auto action) {
            auto camera = mEngine->scene->getActiveCamera();
            if (camera) {
                if (action == Vox::Input::KeyAction::RELEASE) return;
                auto transform = camera->gameObject->transform;
                auto pos = transform->getPos();

                switch (key) {
                    case Vox::Input::Key::W:
                        transform->setPos(pos + transform->getForward());
                        break;
                    case Vox::Input::Key::S:
                        transform->setPos(pos + -transform->getForward());
                        break;
                    case Vox::Input::Key::A:
                        transform->setPos(pos + -transform->getRight());
                        break;
                    case Vox::Input::Key::D:
                        transform->setPos(pos + transform->getRight());
                        break;
                }
            }
        };



        window->onClose += [this](auto w) { mEngine->shutdown(); };
        gui = new Vox::Editor::Gui();
        gui->init(*Vox::Render::WindowManager::Get().getMainWindow(), *mEngine);

        Vox::Platform::LinuxPlatform platform{};
        if (enableSandbox) {
            customSandboxPaths.emplace_back("./");
            platform.enableSandbox(customSandboxPaths);
        }
        mEngine->run();
        cleanup();
    }

    void cleanup() const {
        delete mEngine;
        delete gui;
    }
};
