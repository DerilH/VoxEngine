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
#include <VoxEngine/resources/ResourceManager.h>

#include "GizmoPass.h"
#include "GuiRenderPass.h"
#include "screens/SceneEditScreen.h"
#include "VoxEngine/scene/components/MeshRendererComponent.h"
#include "Theme.h"

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

        LoadEditorFonts(mEngine->getResourceManager()->getResourcesRoot());
        ApplyVoxTheme();

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

        auto targets = mRenderer->getPresentTargets() | std::views::values;
        VkFormat _swapchainImageFormat = VK_FORMAT_UNDEFINED;
        for (const auto &target: targets) {
            _swapchainImageFormat = Render::Vulkan::toVk(target->getBackBuffer()->getFormat());
        }
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &_swapchainImageFormat;

        ImGui_ImplVulkan_Init(&init_info);

        auto &graph = mRenderer->getGraph();
        graph.createTexture("GizmoID", Render::Format::R32G32_UINT);
        auto gizmoPass = new GizmoPass(Render::RenderPassType::GIZMO_PASS, {}, {graph.getTexture("GizmoID"), Vox::Render::PassTransition::DISCARD_W_ATTACHMENT, true}, {graph.getTexture("Scene"), Vox::Render::PassTransition::W_ATTACHMENT_W_ATTACHMENT});
        graph.addPass(gizmoPass);

        auto guiPass = new GuiRenderPass(Render::RenderPassType::UI_PASS, {{graph.getTexture("Scene"), Vox::Render::PassTransition::W_ATTACHMENT_R_SHADER}}, {{graph.getTexture("MainWindow"), Vox::Render::PassTransition::DISCARD_W_ATTACHMENT, true}}, this);
        graph.addPass(guiPass);

        mScreens["SceneEdit"] = makeUPtr<SceneEditScreen>(mEngine, gizmoPass);

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
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_MenuBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

        ImGui::Begin("MainDockSpaceWindow", nullptr, hostWindowFlags);
        ImGui::PopStyleVar(3);

        if (ImGui::BeginMenuBar()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ThemeColors::AccentHover);
            ImGui::TextUnformatted("  VoxEngine  ");
            ImGui::PopStyleColor();

            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) {}
                if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) {}
                if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    mEngine->shutdown();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Project Settings...")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About VoxEngine")) {}
                ImGui::EndMenu();
            }

            // Right-aligned stats badge
            float statsWidth = 220.0f;
            float availX = ImGui::GetContentRegionAvail().x;
            if (availX > statsWidth) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availX - statsWidth);

                float fps = static_cast<float>(mFpsCounter.getFps());
                float frameMs = fps > 0.0f ? 1000.0f / fps : 0.0f;
                ImVec4 fpsColor = (fps >= 55.0f) ? ImVec4(0.3f, 0.85f, 0.4f, 1.0f) :
                                  (fps >= 30.0f) ? ImVec4(0.95f, 0.8f, 0.2f, 1.0f) :
                                                   ImVec4(0.95f, 0.3f, 0.3f, 1.0f);

                ImGui::TextColored(fpsColor, "%.0f FPS", fps);
                ImGui::SameLine();
                ImGui::TextDisabled("(%.1f ms)", frameMs);
                ImGui::SameLine();
                ImGui::TextDisabled("| Vulkan");
            }

            ImGui::EndMenuBar();
        }

        // Top Toolbar
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.15f, 1.0f));
        if (ImGui::BeginChild("##EditorToolbar", ImVec2(0, 36), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            // Left: Gizmo modes
            ImGui::SetCursorPos(ImVec2(10, 5));
            static int activeTool = 0;
            auto ToolButton = [&](const char *label, int id, const char *tooltip) {
                bool active = (activeTool == id);
                if (active) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::Accent);
                }
                if (ImGui::Button(label, ImVec2(28, 26))) {
                    activeTool = id;
                }
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", tooltip);
                if (active) ImGui::PopStyleColor();
                ImGui::SameLine();
            };

            ToolButton("T", 0, "Translate Tool (W)");
            ToolButton("R", 1, "Rotate Tool (E)");
            ToolButton("S", 2, "Scale Tool (R)");

            // Center: Playback Controls
            float toolbarWidth = ImGui::GetWindowWidth();
            float centerButtonsWidth = 110.0f;
            ImGui::SetCursorPos(ImVec2((toolbarWidth - centerButtonsWidth) * 0.5f, 5));

            static bool isPlaying = false;
            static bool isPaused = false;

            ImGui::PushStyleColor(ImGuiCol_Button, isPlaying ? ImVec4(0.2f, 0.65f, 0.3f, 1.0f) : ImVec4(0.18f, 0.20f, 0.25f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.8f, 0.35f, 1.0f));
            if (ImGui::Button(" > ", ImVec2(32, 26))) {
                isPlaying = !isPlaying;
                if (!isPlaying) isPaused = false;
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", isPlaying ? "Stop Simulation" : "Start Simulation");
            ImGui::PopStyleColor(2);
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, isPaused ? ImVec4(0.85f, 0.65f, 0.2f, 1.0f) : ImVec4(0.18f, 0.20f, 0.25f, 1.0f));
            if (ImGui::Button(" || ", ImVec2(32, 26))) {
                if (isPlaying) isPaused = !isPaused;
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pause Simulation");
            ImGui::PopStyleColor();
            ImGui::SameLine();

            if (ImGui::Button(" [] ", ImVec2(32, 26))) {
                isPlaying = false;
                isPaused = false;
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Simulation");

            // Right: Scene status
            float badgeWidth = 140.0f;
            if (toolbarWidth > 400.0f) {
                ImGui::SetCursorPos(ImVec2(toolbarWidth - badgeWidth - 12.0f, 8));
                ImGui::TextDisabled("Scene: ");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, ThemeColors::AccentHover);
                ImGui::TextUnformatted(mEngine->scene ? "Active" : "None");
                ImGui::PopStyleColor();
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);

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
