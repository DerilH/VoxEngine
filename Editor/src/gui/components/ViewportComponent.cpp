//
// Created by deril on 10/13/26.
//

#include "ViewportComponent.h"

#include <algorithm>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat3x3.hpp>
#include <imgui_impl_vulkan.h>
#include <VoxEngine/render/Renderer.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>
#include <VoxEngine/render/windowing/WindowManager.h>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/Scene.h>
#include <VoxEngine/scene/components/CameraComponent.h>
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

        bool isImageHovered = ImGui::IsItemHovered();

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

        if (isImageHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            mIsCaptured = true;
            mJustWarped = false;
        }

        if (mIsCaptured) {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_None);

                constexpr float margin = 12.0f;
                if (viewportSize.x > margin * 3.0f && viewportSize.y > margin * 3.0f) {
                    ImVec2 mousePos = ImGui::GetIO().MousePos;
                    float minX = screenPos.x + margin;
                    float maxX = screenPos.x + viewportSize.x - margin;
                    float minY = screenPos.y + margin;
                    float maxY = screenPos.y + viewportSize.y - margin;

                    float newX = mousePos.x;
                    float newY = mousePos.y;
                    bool needWarp = false;

                    if (mousePos.x <= minX) {
                        newX = maxX - 2.0f;
                        needWarp = true;
                    } else if (mousePos.x >= maxX) {
                        newX = minX + 2.0f;
                        needWarp = true;
                    }

                    if (mousePos.y <= minY) {
                        newY = maxY - 2.0f;
                        needWarp = true;
                    } else if (mousePos.y >= maxY) {
                        newY = minY + 2.0f;
                        needWarp = true;
                    }

                    if (needWarp) {
                        auto mainWin = Vox::Render::WindowManager::Get().getMainWindow();
                        if (mainWin && mainWin->getHandle()) {
                            auto glfwWin = static_cast<GLFWwindow *>(mainWin->getHandle());
                            glfwSetCursorPos(glfwWin, static_cast<double>(newX), static_cast<double>(newY));
                        }
                        ImGui::GetIO().WantSetMousePos = true;
                        ImGui::GetIO().MousePos = ImVec2(newX, newY);
                        mJustWarped = true;
                    }
                }

                auto camera = mEngine->scene ? mEngine->scene->getActiveCamera() : nullptr;
                if (camera && camera->gameObject && camera->gameObject->transform) {
                    auto transform = camera->gameObject->transform;

                    if (mLastCamera != camera) {
                        mAnglesInitialized = false;
                        mLastCamera = camera;
                    }

                    if (!mAnglesInitialized) {
                        glm::vec3 fwd = transform->getForward();
                        if (glm::length(fwd) > 0.001f) {
                            fwd = glm::normalize(fwd);
                            mPitch = glm::degrees(std::asin(std::clamp(fwd.y, -1.0f, 1.0f)));
                            mYaw = glm::degrees(std::atan2(fwd.x, -fwd.z));
                        } else {
                            mPitch = 0.0f;
                            mYaw = 0.0f;
                        }
                        mAnglesInitialized = true;
                    }

                    ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
                    constexpr float sensitivity = 0.15f;

                    if (mJustWarped || std::abs(mouseDelta.x) > viewportSize.x * 0.4f || std::abs(mouseDelta.y) > viewportSize.y * 0.4f) {
                        mJustWarped = false;
                    } else if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
                        mYaw += mouseDelta.x * sensitivity;
                        mPitch += mouseDelta.y * sensitivity;
                        mPitch = std::clamp(mPitch, -89.0f, 89.0f);

                        float yawRad = glm::radians(mYaw);
                        float pitchRad = glm::radians(mPitch);

                        glm::vec3 forward;
                        forward.x = std::sin(yawRad) * std::cos(pitchRad);
                        forward.y = std::sin(pitchRad);
                        forward.z = -std::cos(yawRad) * std::cos(pitchRad);
                        forward = glm::normalize(forward);

                        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
                        glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
                        glm::vec3 up = glm::normalize(glm::cross(right, forward));

                        glm::mat3 rotMat;
                        rotMat[0] = right;
                        rotMat[1] = up;
                        rotMat[2] = -forward;

                        glm::quat newRot = glm::conjugate(glm::quat_cast(rotMat));
                        transform->setRotation(newRot);
                    }

                    float dt = ImGui::GetIO().DeltaTime;
                    if (dt > 0.0f && dt < 0.1f) {
                        float speed = 10.0f * dt;
                        if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) speed *= 2.5f;

                        glm::vec3 forward = transform->getForward();
                        glm::vec3 right = transform->getRight();
                        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
                        glm::vec3 pos = transform->getPos();

                        if (ImGui::IsKeyDown(ImGuiKey_W)) pos += forward * speed;
                        if (ImGui::IsKeyDown(ImGuiKey_S)) pos -= forward * speed;
                        if (ImGui::IsKeyDown(ImGuiKey_D)) pos += right * speed;
                        if (ImGui::IsKeyDown(ImGuiKey_A)) pos -= right * speed;
                        if (ImGui::IsKeyDown(ImGuiKey_E)) pos += up * speed;
                        if (ImGui::IsKeyDown(ImGuiKey_Q)) pos -= up * speed;

                        transform->setPos(pos);
                    }
                }
            } else {
                mIsCaptured = false;
                mJustWarped = false;
            }
        }

        ImDrawList *drawList = ImGui::GetWindowDrawList();
        ImVec2 hudPos = ImVec2(screenPos.x + 12.0f, screenPos.y + 12.0f);
        char hudText[128];
        if (mIsCaptured) {
            snprintf(hudText, sizeof(hudText), " %dx%d  |  Camera: [Captured] Mouse Look + WASD ", (int)viewportSize.x, (int)viewportSize.y);
        } else {
            snprintf(hudText, sizeof(hudText), " %dx%d  |  Camera: Hold LMB to Look + WASD ", (int)viewportSize.x, (int)viewportSize.y);
        }
        ImVec2 hudTextSize = ImGui::CalcTextSize(hudText);
        ImVec2 hudBgMax = ImVec2(hudPos.x + hudTextSize.x + 8.0f, hudPos.y + hudTextSize.y + 8.0f);

        drawList->AddRectFilled(hudPos, hudBgMax, IM_COL32(18, 19, 24, 200), 5.0f);
        drawList->AddRect(hudPos, hudBgMax, IM_COL32(50, 52, 65, 200), 5.0f);
        drawList->AddText(ImVec2(hudPos.x + 4.0f, hudPos.y + 4.0f), IM_COL32(200, 205, 220, 240), hudText);

        ImGui::End();
    }
}
