//
// Created by deril on 10/13/26.
//

#include "ViewportComponent.h"

#include <imgui_impl_vulkan.h>
#include <VoxEngine/render/Renderer.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/components/MeshRendererComponent.h>
#include <VoxEngine/scene/components/Transform.h>
#include <vulkan/vulkan_core.h>
#include "../Theme.h"

namespace Vox::Editor {
    void ViewportComponent::renderInternal(Render::RenderContext context) {
        if (!mRegistered) {
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
            vkCreateSampler(*Render::Vulkan::ResourceCast(context.renderer->getBackend()->getDevice()), &samplerInfo, nullptr, &viewportSampler);

            mTextureId = (ImTextureID) ImGui_ImplVulkan_AddTexture(
                viewportSampler,
                Render::Vulkan::ResourceCast(context.renderer->getGraph().getTexture("Scene")->getExact())->getView(),
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );
            mRegistered = true;
        }

        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport", nullptr, flags);
        ImGui::PopStyleVar();

        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImVec2 screenPos = ImGui::GetCursorScreenPos();

        if (viewportSize.x > 0 && viewportSize.y > 0) {
            ImGui::Image(mTextureId, viewportSize);
        }

        // Overlay HUD
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        ImVec2 hudPos = ImVec2(screenPos.x + 12.0f, screenPos.y + 12.0f);
        char hudText[128];
        snprintf(hudText, sizeof(hudText), " %dx%d  |  Camera: WASD Fly ", (int)viewportSize.x, (int)viewportSize.y);
        ImVec2 hudTextSize = ImGui::CalcTextSize(hudText);
        ImVec2 hudBgMax = ImVec2(hudPos.x + hudTextSize.x + 8.0f, hudPos.y + hudTextSize.y + 8.0f);

        drawList->AddRectFilled(hudPos, hudBgMax, IM_COL32(18, 19, 24, 200), 5.0f);
        drawList->AddRect(hudPos, hudBgMax, IM_COL32(50, 52, 65, 200), 5.0f);
        drawList->AddText(ImVec2(hudPos.x + 4.0f, hudPos.y + 4.0f), IM_COL32(200, 205, 220, 240), hudText);

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                buffer.assign(static_cast<const char *>(payload->Data), payload->DataSize);
                auto asset = context.renderer->getResourceManager()->get<Resources::ModelAsset>(buffer);
                if (asset) {
                    for (int i = 0; i < asset.value()->getNestedCount(); ++i) {
                        auto mesh = asset.value()->getNestedUnsafe<Resources::MeshAsset>(i);
                        auto obj = mEngine->scene->createObject(mesh->getName());
                        auto render = new Scene::MeshRendererComponent(obj);
                        auto defaultMat = context.renderer->getResourceManager()->getDefault<Resources::MaterialAsset>();
                        render->setMaterial(defaultMat);
                        render->setMeshProvider([mesh]() {
                            return mesh;
                        });
                        obj->attachComponent(render);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::End();
    }
}
