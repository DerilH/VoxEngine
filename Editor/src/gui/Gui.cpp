//
// Created by deril on 2/16/26.
//

#include "Gui.h"
#include "VoxEngine/Engine.h"
#include "VoxEngine/render/RenderBackend.h"
#include "VoxEngine/render/vulkan/VulkanBackend.h"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <VoxCore/Time.h>
#include <VoxEngine/render/TextureRenderTarget.h>
#include <VoxEngine/render/vulkan/VulkanState.h>
#include <VoxEngine/render/windowing/Window.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/VulkanUtil.h>
#include <VoxEngine/scene/GameObject.h>
#include "GuiRenderPass.h"
#include "screens/SceneEditScreen.h"
#include "VoxEngine/scene/components/MeshRendererComponent.h"

namespace Vox::Editor {
    void Gui::init(Render::Window &window, Engine &engine) {
        mWindow = &window;
        mEngine = &engine;
        mRenderer = engine.getRenderer();
        VOX_CHECK(mRenderer->backendApi == Render::RenderAPI::VULKAN_API, "Imgui can only be used with Vulkan as render backend for engine");

        if (mInitialized) {
            LOG_ERROR("Gui already initialized");
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.FontGlobalScale = 2;
        ImGui::StyleColorsDark();
        auto &style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

        ImGui_ImplGlfw_InitForVulkan((GLFWwindow *) mWindow->getHandle(), true);
        auto device = Render::Vulkan::ResourceCast(mRenderer->getBackend()->getDevice());

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
            poolInfo.poolSizeCount = 11;
            poolInfo.pPoolSizes = poolSizes;
            poolInfo.maxSets = 100;

            vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &pool);
        }

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = Render::Vulkan::ResourceCast(mRenderer->getBackend())->getVkInstance();
        init_info.PhysicalDevice = device->getPhysicalDevice().getHandle();
        init_info.Device = device->getHandle();

        const Render::Vulkan::Queue &queue = device->getQueues().at(Vox::Render::Vulkan::GRAPHICS_QUEUE);
        init_info.QueueFamily = queue.getFamily().index();
        init_info.Queue = queue.getHandle();
        init_info.DescriptorPool = pool;
        init_info.PipelineInfoMain.Subpass = 0;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.MinImageCount = 2;
        init_info.ImageCount = 3;
        init_info.UseDynamicRendering = true;

        init_info.PipelineInfoMain.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;

        auto targets = mRenderer->getPresentTargets();
        VkFormat _swapchainImageFormat = VK_FORMAT_UNDEFINED;
        if (!targets.empty()) {
            _swapchainImageFormat = Render::Vulkan::toVk(targets[0]->getBackBuffer()->getFormat());
        }
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &_swapchainImageFormat;

        ImGui_ImplVulkan_Init(&init_info);

        auto &graph = mRenderer->getGraph();

        auto guiPass = new GuiRenderPass(Render::RenderPassType::UI_PASS, {{graph.getTexture("Scene"), Vox::Render::PassTransition::W_ATTACHMENT_R_SHADER}}, {{graph.getTexture("Out"), Vox::Render::PassTransition::DISCARD_W_ATTACHMENT}}, this);
        graph.addPass(guiPass);

        mScreens["SceneEdit"] = makeUPtr<SceneEditScreen>(mEngine);

        mInitialized = true;
    }


    void Gui::render(Render::RenderContext context) {
        //TODO: replace with deferred buffers deletion
        //Used for proper buffer deletions
        auto device = Render::Vulkan::ResourceCast(mEngine->getRenderer()->getBackend()->getDevice());
        device->waitIdle();

        mFpsCounter.update(Time::Delta());
        VOX_CHECK(mInitialized, "Gui not initialized");
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        constexpr ImGuiWindowFlags hostWindowFlags =
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus |
                ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

        ImGui::Begin("MainDockSpaceWindow", nullptr, hostWindowFlags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::End();
        ImGui::PopStyleColor();

        for (const auto &screen: mScreens | std::views::values) {
            screen->render(context);
        }

        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();

        const auto *vkCmd = dynamic_cast<Render::Vulkan::VulkanCommandBuffer *>(context.cmdBuffer);
        ImGui_ImplVulkan_RenderDrawData(draw_data, *vkCmd, nullptr);
    }
}
