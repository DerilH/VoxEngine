#pragma once

#include <GLFW/glfw3.h>
#include "VoxEngine/Engine.h"
#include <VoxCore/Time.h>
#include <VoxEngine/render/shaders/ShaderCompiler.h>

#include <VoxEngine/render/shaders/ShaderRepository.h>
#include <VoxEngine/render/vulkan/FrameSync.h>
#include <VoxEngine/render/vulkan/LogicalDevice.h>
#include <VoxEngine/render/vulkan/VulkanState.h>

#include "gui/Gui.h"

class MainApp {
    Vox::Engine*mEngine = nullptr;
    Vox::Editor::Gui *gui = nullptr;

public:
    void run() {
        mEngine = new Vox::Engine("Vox", Vox::Render::VULKAN_API);
        mEngine->init();

         gui = new Vox::Editor::Gui(*mEngine->getWindow("Vox"));
         gui->init();

        mEngine->run();

        cleanup();
    }
    void cleanup() const {
        delete mEngine;
    }
};
