#pragma once

#include "VoxEngine/Engine.h"
#include <VoxEngine/platform/LinuxPlatform.h>
#include <VoxEngine/render/shaders/ShaderCompiler.h>

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

        mEngine = new Vox::Engine("Vox", Vox::Render::VULKAN_API);
        Vox::Platform::LinuxPlatform platform{};

        mEngine->init();

        gui = new Vox::Editor::Gui();
        gui->init(*mEngine->getWindow("Vox"), *mEngine);

        if (enableSandbox) platform.enableSandbox(".");
        mEngine->run();
        cleanup();
    }

    void cleanup() const {
        delete mEngine;
        delete gui;
    }
};
