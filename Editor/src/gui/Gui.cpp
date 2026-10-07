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
#include <GLFW/glfw3.h>
#include "GuiRenderPass.h"
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
        const ImGuiIO &io = ImGui::GetIO();
        (void) io;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)mWindow->getHandle(), true);
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
            auto tempTarget = mRenderer->getBackend()->createWindowTarget(&window);
            _swapchainImageFormat = Render::Vulkan::toVk(tempTarget->getBackBuffer()->getFormat());
        }
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &_swapchainImageFormat;

        ImGui_ImplVulkan_Init(&init_info);

        auto &graph = mEngine->getRenderer()->getGraph();
        auto guiPass = new GuiRenderPass(Vox::Render::RenderPassType::UI_PASS, {{graph.getTexture("Scene"), Vox::Render::PassTransition::W_ATTACHMENT_R_SHADER}}, {{graph.getTexture("Color"), Vox::Render::PassTransition::DISCARD_W_ATTACHMENT}}, this);
        graph.addPass(guiPass);

        mInitialized = true;
    }

    bool registered = false;
    ImTextureID viewportTextureID;
    void Gui::render(Render::RenderContext cmd, Ref<Render::Texture> sceneTexture) {
        //Used for proper buffer deletions
        //TODO: replace with deferred buffers deletion

        auto device = Render::Vulkan::ResourceCast(mEngine->getRenderer()->getBackend()->getDevice());
        device->waitIdle();
        if (!registered) {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;

            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

            samplerInfo.anisotropyEnable = VK_FALSE;
            samplerInfo.maxAnisotropy = 1.0f;

            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 1.0f;
            samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

            VkSampler viewportSampler;
            vkCreateSampler(*device, &samplerInfo, nullptr, &viewportSampler);

            viewportTextureID = (ImTextureID)ImGui_ImplVulkan_AddTexture(
                    viewportSampler,
                    Render::Vulkan::ResourceCast(sceneTexture)->getView(),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                );
            registered = true;
        }


        mFpsCounter.update(Time::Delta());
        VOX_CHECK(mInitialized, "Gui not initialized");
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::Begin("Viewport",nullptr,flags);
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        ImGui::Image(
            viewportTextureID,
            viewportSize
        );
        ImGui::End();
        renderTreePanel();

        renderSelectedOptions();

        mExplorer.render();

        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();

        auto *vkCmd = dynamic_cast<Render::Vulkan::VulkanCommandBuffer *>(cmd.cmdBuffer);
        ImGui_ImplVulkan_RenderDrawData(draw_data, *vkCmd, nullptr);
    }

    void Gui::renderTreePanel() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200.0f, viewport->WorkSize.y * 0.5f), ImGuiCond_Always);
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        ImGui::Begin("Scene", nullptr, windowFlags);

        auto objs = mEngine->scene->getRootObjects();
        for (const auto el: objs) {
            ImGui::PushID(el);

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

            if (mSelected == el) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            auto isOpen = ImGui::TreeNodeEx(el->getName().c_str(), flags);
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
                mSelected = el;
                mMeshRotation = glm::degrees(glm::eulerAngles(mSelected->transform->getRotation()));
            }

            if (isOpen) {
                ImGui::TreePop();
            }
            ImGui::PopID();
        }

        ImGui::End();
    }

    void Gui::renderSelectedOptions() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 pos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - 300, viewport->WorkPos.y);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);

        ImVec2 size = ImVec2(300.0f, viewport->WorkSize.y);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        ImGui::Begin("Object options", nullptr, windowFlags);

        renderTransformOptions();

        renderComponents();

        ImGui::End();
    }

    void Gui::renderTransformOptions() {
        ImGui::Text("Tranform");
        if (auto el = mSelected) {
            auto pos = el->transform->getPos();
            if (ImGui::DragFloat3("Position", &pos.x, 0.05f)) {
                el->transform->setPos(pos);
            }

            if (ImGui::DragFloat3("Rotation", &mMeshRotation.x, 1.0f)) {
                el->transform->setRotation(glm::quat(glm::radians(mMeshRotation)));
            }

            auto scale = el->transform->getScale();
            if (ImGui::DragFloat3("Scale", &scale.x, 0.05f)) {
                el->transform->setScale(scale);
            }
        }
    }


    void Gui::renderComponents() {
        if (!mSelected) return;

        auto renderer = mSelected->findComponent<Scene::MeshRendererComponent>();
        if (!renderer) return;

        auto mat = renderer->getMaterial()->cloneTyped();
        if (!mat) return;
        bool changed = false;
        if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            // auto polyMode = mat->polygonMode;
            // if (RenderEnumCombo("Polygon Mode", polyMode)) {
            //     mat->polygonMode = polyMode;
            //     changed = true;
            // }
            //
            // auto cullMode = mat->cullMode;
            // if (RenderEnumCombo("Cull Mode", cullMode)) {
            //     mat->cullMode = cullMode;
            //     changed = true;
            // }
            //
            // auto topology = mat->topology;
            // if (RenderEnumCombo("Topology", topology)) {
            //     mat->topology = topology;
            //     changed = true;
            // }
            //
            // ImGui::Separator();
            // ImGui::Text("Shaders");
            //
            // for (auto &[stage, shaderPath]: mat->shaders) {
            // std::string stageName = std::string(magic_enum::enum_name(stage));
            // ImGui::PushID(static_cast<int>(stage));

            // char buffer[256];
            // strncpy(buffer, shaderPath.c_str(), sizeof(buffer));
            static char pathBuffer[512] = "";
            strncpy(pathBuffer, renderer->getMaterial()->getPath().c_str(), renderer->getMaterial()->getPath().size());
            if (ImGui::InputText("Material", pathBuffer, IM_ARRAYSIZE(pathBuffer))) {
                // shaderPath = InternedString(buffer);
                changed = true;
            }

            // ImGui::PopID();
            // }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                    const char *draggedPath = static_cast<const char *>(payload->Data);
                    strncpy(pathBuffer, draggedPath, sizeof(pathBuffer) - 1);
                    pathBuffer[sizeof(pathBuffer) - 1] = '\0';
                    changed = true;
                }
                ImGui::EndDragDropTarget();
            }

            try {
                if (changed) {
                    auto newMat = Resources::ResourcesManager::Get().get<Resources::MaterialAsset>(InternedString(pathBuffer));
                    renderer->setMaterial(newMat);
                }
            } catch (std::exception &e) {
            }
        }

    }
}
