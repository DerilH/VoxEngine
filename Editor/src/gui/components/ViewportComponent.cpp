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
        ImGui::Begin("Viewport", nullptr, flags);
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        ImGui::Image(mTextureId, viewportSize);


        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                buffer.assign(static_cast<const char *>(payload->Data), payload->DataSize);
                auto asset = context.renderer->getResourceManager()->get<Resources::ModelAsset>(buffer);
                if (asset) {
                    for (int i = 0; i < asset.value()->getNestedCount(); ++i) {
                        auto mesh = asset.value()->getNestedUnsafe<Resources::MeshAsset>(i);
                        auto obj = mEngine->scene->createObject(mesh->getName());
                        auto render = new Scene::MeshRendererComponent(obj);
                        auto asset = context.renderer->getResourceManager()->getDefault<Resources::MaterialAsset>();
                        render->setMaterial(asset);
                        render->setMeshProvider([mesh]() {
                            return mesh;
                        });
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();
    }
}
