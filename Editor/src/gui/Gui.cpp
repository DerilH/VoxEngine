//
// Created by deril on 2/16/26.
//

#include "Gui.h"

#include <imgui/imgui.h>
#include <imgui/backend/imgui_impl_glfw.h>
#include <imgui/backend/imgui_impl_vulkan.h>
#include <VoxCore/Time.h>
#include <VoxEngine/render/vulkan/VulkanState.h>
#include <VoxEngine/render/windowing/Window.h>

namespace Vox::Editor {
    Gui::Gui(const Render::Windowing::Window &m_window) : mWindow(m_window) {
    }

    void Gui::init() {
        if (mInitialized) {
            LOG_ERROR("Gui already initialized");
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        const ImGuiIO &io = ImGui::GetIO();
        (void) io;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(mWindow.getHandle(), true);
        const Render::Vulkan::VulkanState *mVKstate = Render::Vulkan::VulkanState::Get();

        VkDescriptorPool pool; {
            VkDescriptorPoolSize poolSizes[] = {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
            };

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = 2;
            poolInfo.pPoolSizes = poolSizes;
            poolInfo.maxSets = 100;

            vkCreateDescriptorPool(mVKstate->device->getHandle(), &poolInfo, nullptr, &pool);
        }


        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = mVKstate->instance;
        init_info.PhysicalDevice = mVKstate->physicalDevice->getHandle();
        init_info.Device = mVKstate->device->getHandle();
        const Render::Vulkan::Queue &queue = mVKstate->device->getQueues().at(Vox::Render::Vulkan::GRAPHICS_QUEUE);
        init_info.QueueFamily = queue.getFamily().index();
        init_info.Queue = queue.getHandle();
        init_info.DescriptorPool = pool;
        init_info.PipelineInfoMain.RenderPass = mVKstate->renderPass->getHandle();
        init_info.PipelineInfoMain.Subpass = 0;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.MinImageCount = 2;
        // init_info.ImageCount = mVKstate->surface->getSwapChain().getImageCount();

        ImGui_ImplVulkan_Init(&init_info);
        mInitialized = true;
    }

    void Gui::render(Render::Vulkan::FrameSync &frame) {
        mFpsCounter.update(Time::Delta());
        VOX_CHECK(mInitialized, "Gui not initialized");
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello Vulkan + ImGui");
        float v = 0;
        ImGui::SliderFloat("test", &v, 0, 100);
        if (ImGui::Button("Save")) {
            LOG_INFO("pressed")
        }

        ImGui::Text("HELO ImGui!");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
        ImGui::Text(std::format("FPS: {}", static_cast<int>(mFpsCounter.getFps())).c_str());
#pragma clang diagnostic pop

        ImGui::End();

        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(draw_data, frame.getCmdBuffer().getHandle());
    }
}
