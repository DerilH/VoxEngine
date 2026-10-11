#pragma once

#include "VoxEngine/Engine.h"
#include <VoxEngine/platform/LinuxPlatform.h>
#include <VoxEngine/render/shaders/ShaderCompiler.h>
#include <VoxEngine/render/windowing/WindowManager.h>

#include <VoxEngine/render/vulkan/VulkanState.h>
#include <CLI/CLI.hpp>

#include "gui/Gui.h"

class MainApp {
    Vox::Engine *mEngine = nullptr;
    Vox::Editor::Gui *gui = nullptr;

public:
    void run(int argc, char **argv) {
        CLI::App app{"VoxEngine"};

        bool enableSandbox = false;
        app.add_flag("--enable-sandbox", enableSandbox, "Enables sandbox for protection from unwanted filesystem changes");

        std::string dir = "default";
        app.add_option("--resources", dir, "Set app resources directory")->required();
        app.parse(argc, argv);

        Vox::Resources::ResourcesManager::SetRoot(dir);
        Vox::Render::WindowManager::initBackend();

        mEngine = new Vox::Engine("Vox", Vox::Render::VULKAN_API);
        mEngine->init();

        auto window = Vox::Render::WindowManager::Get().createWindow("Test", {1280, 720}, 0, true);
        Vox::Render::WindowManager::Get().setMainWindow(window);
        auto windowTarget = mEngine->getRenderer()->getBackend()->createWindowTarget(window);
        mEngine->getRenderer()->addViewportTarget("Scene", mEngine->getRenderer()->getBackend()->createTextureTarget(windowTarget->getSize(), Vox::Render::Format::BGRA8));
        mEngine->getRenderer()->addPresentTarget(windowTarget);
        mEngine->getRenderer()->createGraph();

        auto& graph = mEngine->getRenderer()->getGraph();
        auto outText = graph.createTexture("Out");
        outText->setExact([windowTarget]() {return windowTarget->getBackBuffer();});

        window->setOnClose([this](){mEngine->shutdown();});
        gui = new Vox::Editor::Gui();
        gui->init(*Vox::Render::WindowManager::Get().getMainWindow(), *mEngine);

        Vox::Platform::LinuxPlatform platform{};
        if (enableSandbox) platform.enableSandbox(".");
        mEngine->run();
        cleanup();
    }

    void cleanup() const {
        delete mEngine;
        delete gui;
    }
};
