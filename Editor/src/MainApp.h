#pragma once

#include "VoxEngine/Engine.h"
#include <VoxEngine/platform/LinuxPlatform.h>
#include <VoxEngine/render/shaders/ShaderCompiler.h>
#include <VoxEngine/render/windowing/WindowManager.h>

#include <VoxEngine/render/vulkan/VulkanState.h>
#include <CLI/CLI.hpp>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/components/Transform.h>

#include "assets/AssetConverter.h"
#include "assets/AssetManager.h"
#include "gui/Gui.h"

class MainApp {
    Vox::Engine *mEngine = nullptr;
    Vox::Editor::Gui *gui = nullptr;
    Vox::Editor::Assets::AssetManager mAssetManager;

public:
    int run(int argc, char **argv) {
        CLI::App app{"VoxEngine"};
        app.allow_extras();

        std::vector<std::filesystem::path> customSandboxPaths;
        bool enableSandbox = false;
        app.add_flag("--enable-sandbox", enableSandbox, "Enables sandbox for protection from unwanted filesystem changes");

        app.add_option("--sandbox-paths", customSandboxPaths,"Custom directories allowed in sandbox mode")->expected(1, -1);
        std::string dir = "default";
        app.add_option("--resources", dir, "Set app resources directory")->required();
        uint32_t monitor = 0;
        app.add_option("--monitor", monitor, "Set on which monitor editor window should be created");
        bool maximized = false;
        app.add_flag("--maximized", maximized, "If set editor window will be maximized on start");
        try {
            app.parse(argc, argv);
        } catch (const CLI::ParseError &e) {
            return app.exit(e);
        }
        for (auto& s : app.remaining())
            LOG_WARN("Ignored arg: {}", s);

        Vox::Render::WindowManager::initBackend();

        mAssetManager.setPaths(dir,dir / std::filesystem::path("../.cache"));
        mAssetManager.loadAll();

        mEngine = new Vox::Engine("Vox", Vox::Render::VULKAN_API);
        mEngine->init();
        Vox::Editor::Assets::AssetConverter::convertAll(&mAssetManager, mEngine->getResourceManager());

        auto window = Vox::Render::WindowManager::Get().createWindow("Test", {1280, 720}, monitor, maximized);
        Vox::Render::WindowManager::Get().setMainWindow(window);
        auto windowTarget = mEngine->getRenderer()->getBackend()->createWindowTarget(window);
        mEngine->getRenderer()->addViewportTarget("Scene", mEngine->getRenderer()->getBackend()->createTextureTarget(windowTarget->getSize(), Vox::Render::Format::BGRA8));
        mEngine->getRenderer()->addPresentTarget("MainWindow", windowTarget);
        mEngine->getRenderer()->createGraph();

        window->onClose += [this](auto w) { mEngine->shutdown(); };
        gui = new Vox::Editor::Gui();
        gui->init(*Vox::Render::WindowManager::Get().getMainWindow(), *mEngine, mAssetManager);

        Vox::Platform::LinuxPlatform platform{};
        if (enableSandbox) {
            customSandboxPaths.emplace_back("./");
            platform.enableSandbox(customSandboxPaths);
        }
        mEngine->run();
        cleanup();
        return 0;
    }

    void cleanup() const {
        delete mEngine;
        delete gui;
    }
};
