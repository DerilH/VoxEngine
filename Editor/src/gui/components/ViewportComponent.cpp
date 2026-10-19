//
// Created by deril on 10/13/26.
//

#include "ViewportComponent.h"

#include <algorithm>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
    static glm::vec3 computeMeshAxis(Ref<Resources::MeshAsset> mesh) {
        if (!mesh) return glm::vec3(0.0f);
        const auto &vertices = mesh->getVertices();
        if (vertices.empty()) return glm::vec3(0.0f);
        glm::vec3 center(0.0f);
        for (const auto &v : vertices) {
            center += v;
        }
        center /= static_cast<float>(vertices.size());
        if (glm::length(center) > 1e-4f) {
            return glm::normalize(center);
        }
        return glm::vec3(0.0f);
    }

    ViewportComponent::ViewportComponent(Ref<Engine> mEngine, Ref<GizmoPass> gizmoPass) : mEngine(mEngine), mGizmoPass(gizmoPass) {
        mGizmoPass->onPickComplete += [mEngine, this](auto index) {
            LOG_VERBOSE("Picked: {}", index);
            if (isGizmoElement(index)) {
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    mActiveGizmoElement = static_cast<GizmoElement>(index);
                    mIsDraggingGizmo = true;
                    mJustStartedDrag = true;
                }
                return;
            }

            mActiveGizmoElement = GizmoElement::None;
            mIsDraggingGizmo = false;
            mJustStartedDrag = false;
            auto obj = mEngine->getGameObjectsRegistry()->find(index);
            onObjectPicked.invoke(obj);
        };

        mEngine->getRenderer()->onCollect += [this](Ref<Render::Renderer> a) {
            auto obj = mSelectedObj;
            if (!obj) return;

            auto gizmoModel = a->getResourceManager()->get<Resources::ModelAsset>("editor/gizmo.fbx");
            if (!gizmoModel) return;

            auto gizmoMat = a->getResourceManager()->get<Resources::MaterialAsset>("editor/gizmo.vmat");
            if (!gizmoMat) return;

            auto mat = obj->transform->computeWorldMatrix();
            for (int i = 0; i < gizmoModel.value()->getNestedCount(); ++i) {
                auto gizmoMesh = gizmoModel.value()->getMesh(i);
                if (!gizmoMesh) continue;
                auto name = gizmoMesh.value()->getName();
                uint64_t index = 0;
                if (name == "Arrow_X") {
                    index = static_cast<uint64_t>(GizmoElement::Arrow_X);
                    auto axis = computeMeshAxis(gizmoMesh.value());
                    if (glm::length(axis) > 0.5f) mArrowDirs[0] = axis;
                } else if (name == "Arrow_Y") {
                    index = static_cast<uint64_t>(GizmoElement::Arrow_Y);
                    auto axis = computeMeshAxis(gizmoMesh.value());
                    if (glm::length(axis) > 0.5f) mArrowDirs[1] = axis;
                } else if (name == "Arrow_Z") {
                    index = static_cast<uint64_t>(GizmoElement::Arrow_Z);
                    auto axis = computeMeshAxis(gizmoMesh.value());
                    if (glm::length(axis) > 0.5f) mArrowDirs[2] = axis;
                } else if (name == "Ring_X") {
                    index = static_cast<uint64_t>(GizmoElement::Ring_X);
                } else if (name == "Ring_Y") {
                    index = static_cast<uint64_t>(GizmoElement::Ring_Y);
                } else if (name == "Ring_Z") {
                    index = static_cast<uint64_t>(GizmoElement::Ring_Z);
                } else if (name == "Cube_X") {
                    index = static_cast<uint64_t>(GizmoElement::Cube_X);
                } else if (name == "Cube_Y") {
                    index = static_cast<uint64_t>(GizmoElement::Cube_Y);
                } else if (name == "Cube_Z") {
                    index = static_cast<uint64_t>(GizmoElement::Cube_Z);
                }
                a->draw(gizmoMesh.value(), gizmoMat.value(), mat, static_cast<uint8_t>(Scene::RenderLayer::GIZMO), index);
            }
        };
    }

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

        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            mActiveGizmoElement = GizmoElement::None;
            mIsDraggingGizmo = false;
            mJustStartedDrag = false;
        }

        if (isImageHovered) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !mIsDraggingGizmo) {
                mClickMousePos = ImGui::GetMousePos();
                ImVec2 mousePos = mClickMousePos;
                auto extent = Render::Vulkan::ResourceCast(context.renderer->getGraph().getTexture("Scene")->getExact())->getExtent();
                uint32_t pickX = std::clamp((mousePos.x - screenPos.x) / viewportSize.x, 0.0f, 1.0f) * extent.width;
                uint32_t pickY = std::clamp((mousePos.y - screenPos.y) / viewportSize.y, 0.0f, 1.0f) * extent.height;

                if (mGizmoPass) {
                    mGizmoPass->schedulePick(true, {pickX, pickY});
                    LOG_VERBOSE("Viewport pick at: {}, {}", pickX, pickY);
                }
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                mIsCaptured = true;
                mJustWarped = false;
                mIsDraggingGizmo = false;
                mActiveGizmoElement = GizmoElement::None;
            }
        }

        if (mIsDraggingGizmo && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

            if (mSelectedObj && mSelectedObj->transform) {
                glm::vec3 localDir(0.0f);
                if (mActiveGizmoElement == GizmoElement::Arrow_X) localDir = mArrowDirs[0];
                else if (mActiveGizmoElement == GizmoElement::Arrow_Y) localDir = mArrowDirs[1];
                else if (mActiveGizmoElement == GizmoElement::Arrow_Z) localDir = mArrowDirs[2];

                if (glm::length(localDir) > 1e-4f) {
                    glm::vec3 worldDir = mSelectedObj->transform->getRotation() * localDir;
                    if (glm::length(worldDir) > 1e-4f) {
                        worldDir = glm::normalize(worldDir);
                    } else {
                        worldDir = localDir;
                    }

                    auto camera = mEngine->scene ? mEngine->scene->getActiveCamera() : nullptr;
                    if (camera && camera->gameObject && camera->gameObject->transform) {
                        auto camTransform = camera->gameObject->transform;
                        glm::vec3 camPos = camTransform->getPos();
                        glm::vec3 camFwd = camTransform->getForward();
                        glm::vec3 camUp = camTransform->getUp();
                        glm::mat4 view = glm::lookAt(camPos, camPos + camFwd, camUp);

                        float fov = glm::radians(45.0f);
                        float aspect = 1400.0f / 900.0f;
                        float zNear = 0.1f;
                        float zFar = 100.0f;
                        glm::mat4 proj = glm::perspective(fov, aspect, zNear, zFar);

                        glm::vec3 objPos = mSelectedObj->transform->getPos();

                        auto projectToScreen = [&](const glm::vec3 &worldPos) -> std::pair<glm::vec2, bool> {
                            glm::vec4 clip = proj * view * glm::vec4(worldPos, 1.0f);
                            if (clip.w <= 0.0001f) {
                                return {glm::vec2(0.0f), false};
                            }
                            glm::vec2 ndc = glm::vec2(clip.x, clip.y) / clip.w;
                            glm::vec2 screenCoord = glm::vec2(screenPos.x, screenPos.y) +
                                                   (ndc * 0.5f + 0.5f) * glm::vec2(viewportSize.x, viewportSize.y);
                            return {screenCoord, true};
                        };

                        auto [screenP0, valid0] = projectToScreen(objPos);
                        auto [screenP1, valid1] = projectToScreen(objPos + worldDir);

                        if (valid0 && valid1) {
                            glm::vec2 screenAxis = screenP1 - screenP0;
                            float screenAxisLenSq = glm::dot(screenAxis, screenAxis);

                            glm::vec2 mouseDeltaVec(0.0f);
                            if (mJustStartedDrag) {
                                mouseDeltaVec = glm::vec2(ImGui::GetMousePos().x - mClickMousePos.x,
                                                         ImGui::GetMousePos().y - mClickMousePos.y);
                                mJustStartedDrag = false;
                            } else {
                                ImVec2 delta = ImGui::GetIO().MouseDelta;
                                mouseDeltaVec = glm::vec2(delta.x, delta.y);
                            }

                            if (screenAxisLenSq > 1.0f && (mouseDeltaVec.x != 0.0f || mouseDeltaVec.y != 0.0f)) {
                                float dragAmount = glm::dot(mouseDeltaVec, screenAxis) / screenAxisLenSq;
                                if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
                                    dragAmount *= 0.2f;
                                }
                                glm::vec3 newPos = objPos + worldDir * dragAmount;
                                mSelectedObj->transform->setPos(newPos);
                            }
                        }
                    }
                }
            }
        }

        if (mIsCaptured) {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
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
            snprintf(hudText, sizeof(hudText), " %dx%d  |  Camera: [Captured] Mouse Look + WASD ", (int) viewportSize.x, (int) viewportSize.y);
        } else if (mIsDraggingGizmo) {
            const char *elemName = "Gizmo";
            if (mActiveGizmoElement == GizmoElement::Arrow_X) elemName = "Arrow X";
            else if (mActiveGizmoElement == GizmoElement::Arrow_Y) elemName = "Arrow Y";
            else if (mActiveGizmoElement == GizmoElement::Arrow_Z) elemName = "Arrow Z";
            snprintf(hudText, sizeof(hudText), " %dx%d  |  Move: [%s] Dragging (Shift for precision) ", (int) viewportSize.x, (int) viewportSize.y, elemName);
        } else {
            snprintf(hudText, sizeof(hudText), " %dx%d  |  Camera: Hold RMB to Look + WASD ", (int) viewportSize.x, (int) viewportSize.y);
        }
        ImVec2 hudTextSize = ImGui::CalcTextSize(hudText);
        ImVec2 hudBgMax = ImVec2(hudPos.x + hudTextSize.x + 8.0f, hudPos.y + hudTextSize.y + 8.0f);

        drawList->AddRectFilled(hudPos, hudBgMax, IM_COL32(18, 19, 24, 200), 5.0f);
        drawList->AddRect(hudPos, hudBgMax, IM_COL32(50, 52, 65, 200), 5.0f);
        drawList->AddText(ImVec2(hudPos.x + 4.0f, hudPos.y + 4.0f), IM_COL32(200, 205, 220, 240), hudText);

        ImGui::End();
    }
}
