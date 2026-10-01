//
// Created by deril on 2/16/26.
//

#include "Gui.h"
#include "VoxEngine/Engine.h"
#include "VoxEngine/render/RenderBackend.h"
#include "VoxEngine/render/vulkan/VulkanBackend.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <glm/gtc/matrix_transform.hpp>
#include <VoxCore/Time.h>
#include <VoxEngine/render/vulkan/VulkanState.h>
#include <VoxEngine/render/windowing/Window.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/VulkanUtil.h>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/components/Transform.h>

#include "GuiRenderPass.h"

namespace Vox::Editor {
    void Gui::init(Render::Windowing::Window &window, Engine &engine) {
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
        const ImGuiIO &io = ImGui::GetIO();
        (void) io;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(mWindow->getHandle(), true);
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

        auto targets = mRenderer->getRenderTargets();
        VkFormat _swapchainImageFormat = VK_FORMAT_UNDEFINED;
        if (!targets.empty()) {
            _swapchainImageFormat = Render::Vulkan::toVk(targets[0]->getBackBuffer()->getFormat());
        } else {
            // Fallback if no target yet
            auto tempTarget = mRenderer->getBackend()->createWindowTarget(window.getExtent(), window.getHandle());
            _swapchainImageFormat = Render::Vulkan::toVk(tempTarget->getBackBuffer()->getFormat());
        }
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &_swapchainImageFormat;

        ImGui_ImplVulkan_Init(&init_info);

        auto &graph = mEngine->getRenderer()->getGraph();
        auto guiPass = new GuiRenderPass(Vox::Render::RenderPassType::UI_PASS, {{graph.getTexture("Color"), Vox::Render::PassTransition::NONE_W_ATTACHMENT}}, {}, this);
        graph.addPass(guiPass);

        mInitialized = true;
    }

    void Gui::render(Render::RenderContext cmd) {
        mFpsCounter.update(Time::Delta());
        VOX_CHECK(mInitialized, "Gui not initialized");
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Mesh Control");

        ImGui::Text(std::format("FPS: {}", static_cast<int>(mFpsCounter.getFps())).c_str());
        renderTreePanel();
        ImGui::End();

        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();

        auto *vkCmd = dynamic_cast<Render::Vulkan::VulkanCommandBuffer *>(cmd.cmdBuffer);
        ImGui_ImplVulkan_RenderDrawData(draw_data, *vkCmd, nullptr);
    }

    void Gui::renderTreePanel() {
        ImGui::Begin("Scene");

        auto objs = mEngine->scene->getRootObjects();
        for (const auto el: objs) {
            ImGui::PushID(el);

            if (ImGui::TreeNode(el->getName().c_str())) {
                auto pos = el->transform->getPos();
                if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
                    el->transform->setPos(pos);
                }

                auto rot = el->transform->getRotation();
                glm::vec3 angles =  glm::eulerAngles(rot) * 57.2958f;
                if (ImGui::DragFloat3("Rotation", &angles.x, 1.0f)) {
                    el->transform->setRotation(glm::quat(angles / 57.2958f));
                }

                auto scale = el->transform->getScale();
                if (ImGui::DragFloat3("Scale", &scale.x, 0.05f)) {
                    el->transform->setScale(scale);
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::End();
    }
}
