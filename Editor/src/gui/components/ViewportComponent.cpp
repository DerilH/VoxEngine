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
#include "../../assets/AssetManager.h"
#include "../../assets/AssetConverter.h"
#include "../Gui.h"
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/resources/ModelResource.h>
#include <VoxEngine/resources/MaterialResource.h>
#include <VoxEngine/resources/MeshResource.h>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/Scene.h>
#include <VoxEngine/scene/components/CameraComponent.h>
#include <VoxEngine/scene/components/MeshRendererComponent.h>
#include <VoxEngine/scene/components/Transform.h>
#include <vulkan/vulkan_core.h>
#include "../Theme.h"

namespace Vox::Editor {
    static glm::vec3 computeMeshAxis(ConstRef<Resources::MeshResource> mesh) {
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

            auto resMgr = a->getResourceManager();
            auto gizmoModelHandle = resMgr->find<Resources::ModelResource>("editor/gizmo.fbx");
            if (!gizmoModelHandle) return;
            auto gizmoModel = resMgr->get(*gizmoModelHandle);
            if (!gizmoModel) return;

            auto gizmoMatHandle = resMgr->find<Resources::MaterialResource>("editor/gizmo.vmat");
            if (!gizmoMatHandle) return;
            auto gizmoMat = resMgr->get(*gizmoMatHandle);
            if (!gizmoMat) return;

            auto mat = glm::translate(glm::mat4(1.0f), obj->transform->getPos()) * glm::mat4_cast(obj->transform->getRotation());
            for (uint32_t i = 0; i < gizmoModel.value()->getMeshCount(); ++i) {
                auto meshHandle = gizmoModel.value()->getMesh(i);
                auto gizmoMesh = resMgr->get(meshHandle);
                if (!gizmoMesh) continue;
                std::string name = (i < gizmoModel.value()->getMeshNames().size()) ? gizmoModel.value()->getMeshNames()[i].c_str() : "";
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
                    auto axis = computeMeshAxis(gizmoMesh.value());
                    if (glm::length(axis) > 0.5f && glm::length(mArrowDirs[0]) < 0.5f) mArrowDirs[0] = axis;
                } else if (name == "Cube_Y") {
                    index = static_cast<uint64_t>(GizmoElement::Cube_Y);
                    auto axis = computeMeshAxis(gizmoMesh.value());
                    if (glm::length(axis) > 0.5f && glm::length(mArrowDirs[1]) < 0.5f) mArrowDirs[1] = axis;
                } else if (name == "Cube_Z") {
                    index = static_cast<uint64_t>(GizmoElement::Cube_Z);
                    auto axis = computeMeshAxis(gizmoMesh.value());
                    if (glm::length(axis) > 0.5f && glm::length(mArrowDirs[2]) < 0.5f) mArrowDirs[2] = axis;
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
                auto assetMgr = Gui::GetAssetManager();
                if (assetMgr) {
                    UUID droppedUuid{};
                    if (payload->DataSize == sizeof(UUID)) {
                        droppedUuid = *static_cast<const UUID *>(payload->Data);
                    } else {
                        buffer.assign(static_cast<const char *>(payload->Data), payload->DataSize);
                        droppedUuid = assetMgr->getUUID(buffer.c_str());
                        // if (!droppedUuid.isValid()) droppedUuid = UUID::fromString(buffer.c_str());
                    }

                    if (droppedUuid.isValid()) {
                        auto meta = assetMgr->getMetadata(droppedUuid);
                        if (meta && meta->type == AssetType::MODEL) {
                            auto resMgr = mEngine->getResourceManager();
                            auto modelRes = resMgr->findByAsset<Resources::ModelResource>(droppedUuid);
                            if (!modelRes) {
                                auto modelAsset = assetMgr->getOrLoad<Assets::ModelAsset>(droppedUuid);
                                if (modelAsset) {
                                    auto handle = Assets::AssetConverter::toResource(*modelAsset, resMgr, droppedUuid);
                                    if (handle) modelRes = *handle;
                                }
                            }
                            if (modelRes) {
                                auto model = resMgr->get(*modelRes);
                                if (model) {
                                    UUID defaultMatUuid = assetMgr->getUUID("defaults/DefaultMaterial.vmat");
                                    ConstRef<Resources::MaterialResource> defaultMatRes = nullptr;
                                    if (defaultMatUuid.isValid()) {
                                        auto defaultMatHandle = resMgr->findByAsset<Resources::MaterialResource>(defaultMatUuid);
                                        if (defaultMatHandle) {
                                            auto mR = resMgr->get(*defaultMatHandle);
                                            if (mR) defaultMatRes = mR.value();
                                        }
                                    }

                                    for (uint32_t i = 0; i < model.value()->getMeshCount(); ++i) {
                                        auto meshHandle = model.value()->getMesh(i);
                                        auto obj = mEngine->scene->createObject(meta->fileName.c_str());
                                        auto render = new Scene::MeshRendererComponent(obj);
                                        if (defaultMatRes) {
                                            render->setMaterial(defaultMatRes, defaultMatUuid);
                                        }
                                        render->setMeshProvider([resMgr, meshHandle]() -> ConstRef<Resources::MeshResource> {
                                            auto m = resMgr->get(meshHandle);
                                            return m ? m.value() : nullptr;
                                        }, droppedUuid);
                                        obj->attachComponent(render);
                                    }
                                }
                            }
                        }
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

        if (!ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
            mIsPanning = false;
        }

        if (isImageHovered) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !mIsDraggingGizmo && !mIsPanning) {
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
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
                mIsPanning = true;
                mIsCaptured = false;
                mIsDraggingGizmo = false;
                mActiveGizmoElement = GizmoElement::None;
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                mIsCaptured = true;
                mIsPanning = false;
                mJustWarped = false;
                mIsDraggingGizmo = false;
                mActiveGizmoElement = GizmoElement::None;
            }
        }

        if (mIsPanning) {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

                ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
                if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
                    auto camera = mEngine->scene ? mEngine->scene->getActiveCamera() : nullptr;
                    if (camera && camera->gameObject && camera->gameObject->transform) {
                        auto transform = camera->gameObject->transform;

                        float dist = 10.0f;
                        if (mSelectedObj && mSelectedObj->transform) {
                            dist = glm::length(mSelectedObj->transform->getPos() - transform->getPos());
                            dist = std::clamp(dist, 1.0f, 500.0f);
                        }
                        float fov = glm::radians(45.0f);
                        float worldHeightAtDist = 2.0f * dist * std::tan(fov * 0.5f);
                        float panSpeed = (viewportSize.y > 0.0f) ? (worldHeightAtDist / viewportSize.y) : 0.015f;

                        if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) {
                            panSpeed *= 2.5f;
                        } else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                            panSpeed *= 0.2f;
                        }

                        glm::vec3 right = transform->getRight();
                        glm::vec3 up = transform->getUp();
                        glm::vec3 pos = transform->getPos();

                        pos += (-right * mouseDelta.x + -up * mouseDelta.y) * panSpeed;
                        transform->setPos(pos);
                    }
                }
            } else {
                mIsPanning = false;
            }
        }

        if (mIsDraggingGizmo && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

            if (mSelectedObj && mSelectedObj->transform) {
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

                    glm::vec2 mouseDeltaVec(0.0f);
                    ImVec2 prevMousePos;
                    if (mJustStartedDrag) {
                        prevMousePos = mClickMousePos;
                        mouseDeltaVec = glm::vec2(ImGui::GetMousePos().x - mClickMousePos.x,
                                                 ImGui::GetMousePos().y - mClickMousePos.y);
                        mJustStartedDrag = false;
                    } else {
                        ImVec2 delta = ImGui::GetIO().MouseDelta;
                        prevMousePos = ImVec2(ImGui::GetMousePos().x - delta.x, ImGui::GetMousePos().y - delta.y);
                        mouseDeltaVec = glm::vec2(delta.x, delta.y);
                    }

                    if (isTranslateGizmoElement(mActiveGizmoElement)) {
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

                            auto [screenP0, valid0] = projectToScreen(objPos);
                            auto [screenP1, valid1] = projectToScreen(objPos + worldDir);

                            if (valid0 && valid1) {
                                glm::vec2 screenAxis = screenP1 - screenP0;
                                float screenAxisLenSq = glm::dot(screenAxis, screenAxis);

                                if (screenAxisLenSq > 1.0f && (mouseDeltaVec.x != 0.0f || mouseDeltaVec.y != 0.0f)) {
                                    float dragAmount = glm::dot(mouseDeltaVec, screenAxis) / screenAxisLenSq;
                                    if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) {
                                        dragAmount *= 0.2f;
                                    }
                                    glm::vec3 newPos = objPos + worldDir * dragAmount;
                                    mSelectedObj->transform->setPos(newPos);
                                }
                            }
                        }
                    } else if (isScaleGizmoElement(mActiveGizmoElement)) {
                        glm::vec3 localDir(0.0f);
                        int axisIdx = 0;
                        if (mActiveGizmoElement == GizmoElement::Cube_X) { localDir = mArrowDirs[0]; axisIdx = 0; }
                        else if (mActiveGizmoElement == GizmoElement::Cube_Y) { localDir = mArrowDirs[1]; axisIdx = 1; }
                        else if (mActiveGizmoElement == GizmoElement::Cube_Z) { localDir = mArrowDirs[2]; axisIdx = 2; }

                        if (glm::length(localDir) > 1e-4f) {
                            glm::vec3 worldDir = mSelectedObj->transform->getRotation() * localDir;
                            if (glm::length(worldDir) > 1e-4f) {
                                worldDir = glm::normalize(worldDir);
                            } else {
                                worldDir = localDir;
                            }

                            auto [screenP0, valid0] = projectToScreen(objPos);
                            auto [screenP1, valid1] = projectToScreen(objPos + worldDir);

                            if (valid0 && valid1) {
                                glm::vec2 screenAxis = screenP1 - screenP0;
                                float screenAxisLenSq = glm::dot(screenAxis, screenAxis);

                                if (screenAxisLenSq > 1.0f && (mouseDeltaVec.x != 0.0f || mouseDeltaVec.y != 0.0f)) {
                                    float dragAmount = glm::dot(mouseDeltaVec, screenAxis) / screenAxisLenSq;
                                    if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) {
                                        dragAmount *= 0.2f;
                                    }
                                    glm::vec3 scale = mSelectedObj->transform->getScale();
                                    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                                        scale += glm::vec3(dragAmount);
                                    } else {
                                        scale[axisIdx] += dragAmount;
                                    }
                                    scale = glm::max(scale, glm::vec3(0.001f));
                                    mSelectedObj->transform->setScale(scale);
                                }
                            }
                        }
                    } else if (isRotateGizmoElement(mActiveGizmoElement)) {
                        glm::vec3 localAxis(0.0f);
                        if (mActiveGizmoElement == GizmoElement::Ring_X) localAxis = mArrowDirs[0];
                        else if (mActiveGizmoElement == GizmoElement::Ring_Y) localAxis = mArrowDirs[1];
                        else if (mActiveGizmoElement == GizmoElement::Ring_Z) localAxis = mArrowDirs[2];

                        if (glm::length(localAxis) > 1e-4f) {
                            glm::vec3 worldAxis = mSelectedObj->transform->getRotation() * localAxis;
                            if (glm::length(worldAxis) > 1e-4f) {
                                worldAxis = glm::normalize(worldAxis);
                            } else {
                                worldAxis = localAxis;
                            }

                            if (mouseDeltaVec.x != 0.0f || mouseDeltaVec.y != 0.0f) {
                                auto getRayDir = [&](const ImVec2 &mPos) -> glm::vec3 {
                                    glm::vec2 ndc = ((glm::vec2(mPos.x, mPos.y) - glm::vec2(screenPos.x, screenPos.y)) /
                                                     glm::vec2(viewportSize.x, viewportSize.y)) * 2.0f - 1.0f;
                                    glm::mat4 invVP = glm::inverse(proj * view);
                                    glm::vec4 nearP = invVP * glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
                                    glm::vec4 farP = invVP * glm::vec4(ndc.x, ndc.y, 1.0f, 1.0f);
                                    if (std::abs(nearP.w) > 1e-5f) nearP /= nearP.w;
                                    if (std::abs(farP.w) > 1e-5f) farP /= farP.w;
                                    return glm::normalize(glm::vec3(farP - nearP));
                                };

                                glm::vec3 rayDirPrev = getRayDir(prevMousePos);
                                glm::vec3 rayDirCurr = getRayDir(ImGui::GetMousePos());

                                float denomPrev = glm::dot(rayDirPrev, worldAxis);
                                float denomCurr = glm::dot(rayDirCurr, worldAxis);

                                float deltaAngle = 0.0f;
                                bool rotated = false;

                                if (std::abs(denomPrev) > 0.08f && std::abs(denomCurr) > 0.08f) {
                                    float tPrev = glm::dot(objPos - camPos, worldAxis) / denomPrev;
                                    float tCurr = glm::dot(objPos - camPos, worldAxis) / denomCurr;
                                    if (tPrev > 0.0f && tCurr > 0.0f) {
                                        glm::vec3 hitPrev = (camPos + tPrev * rayDirPrev) - objPos;
                                        glm::vec3 hitCurr = (camPos + tCurr * rayDirCurr) - objPos;
                                        if (glm::length(hitPrev) > 1e-4f && glm::length(hitCurr) > 1e-4f) {
                                            glm::vec3 vP = glm::normalize(hitPrev);
                                            glm::vec3 vC = glm::normalize(hitCurr);
                                            glm::vec3 crossVec = glm::cross(vP, vC);
                                            float sinA = glm::dot(worldAxis, crossVec);
                                            float cosA = glm::clamp(glm::dot(vP, vC), -1.0f, 1.0f);
                                            deltaAngle = std::atan2(sinA, cosA);
                                            deltaAngle = std::clamp(deltaAngle, -0.5f, 0.5f);
                                            rotated = true;
                                        }
                                    }
                                }

                                if (!rotated) {
                                    auto [screenP0, valid0] = projectToScreen(objPos);
                                    auto [screenP1, valid1] = projectToScreen(objPos + worldAxis);
                                    if (valid0 && valid1) {
                                        glm::vec2 screenAxis = screenP1 - screenP0;
                                        float axisLen = glm::length(screenAxis);
                                        if (axisLen > 1e-4f) {
                                            screenAxis /= axisLen;
                                        } else {
                                            screenAxis = glm::vec2(0.0f, 1.0f);
                                        }
                                        glm::vec2 screenTangent(-screenAxis.y, screenAxis.x);
                                        float tangentDist = glm::dot(mouseDeltaVec, screenTangent);

                                        glm::vec3 camToObj = glm::normalize(objPos - camPos);
                                        glm::vec3 frontTangent = glm::cross(worldAxis, -camToObj);
                                        if (glm::length(frontTangent) > 1e-4f) {
                                            auto [screenFront, validF] = projectToScreen(objPos + frontTangent);
                                            if (validF) {
                                                glm::vec2 projFront = screenFront - screenP0;
                                                if (glm::dot(projFront, screenTangent) < 0.0f) {
                                                    tangentDist = -tangentDist;
                                                }
                                            }
                                        }

                                        deltaAngle = tangentDist * 0.01f;
                                        deltaAngle = std::clamp(deltaAngle, -0.5f, 0.5f);
                                        rotated = true;
                                    }
                                }

                                if (rotated && std::abs(deltaAngle) > 1e-6f) {
                                    if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) {
                                        deltaAngle *= 0.2f;
                                    }
                                    glm::quat deltaRot = glm::angleAxis(deltaAngle, worldAxis);
                                    mSelectedObj->transform->setRotation(glm::normalize(deltaRot * mSelectedObj->transform->getRotation()));
                                }
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
        } else if (mIsPanning) {
            snprintf(hudText, sizeof(hudText), " %dx%d  |  Camera: [Pan] MMB Drag (Shift: Fast, Ctrl: Fine) ", (int) viewportSize.x, (int) viewportSize.y);
        } else if (mIsDraggingGizmo) {
            const char *elemName = "Gizmo";
            const char *actionName = "Transform";
            if (mActiveGizmoElement == GizmoElement::Arrow_X) { elemName = "Arrow X"; actionName = "Move"; }
            else if (mActiveGizmoElement == GizmoElement::Arrow_Y) { elemName = "Arrow Y"; actionName = "Move"; }
            else if (mActiveGizmoElement == GizmoElement::Arrow_Z) { elemName = "Arrow Z"; actionName = "Move"; }
            else if (mActiveGizmoElement == GizmoElement::Cube_X) { elemName = "Cube X"; actionName = "Scale"; }
            else if (mActiveGizmoElement == GizmoElement::Cube_Y) { elemName = "Cube Y"; actionName = "Scale"; }
            else if (mActiveGizmoElement == GizmoElement::Cube_Z) { elemName = "Cube Z"; actionName = "Scale"; }
            else if (mActiveGizmoElement == GizmoElement::Ring_X) { elemName = "Ring X"; actionName = "Rotate"; }
            else if (mActiveGizmoElement == GizmoElement::Ring_Y) { elemName = "Ring Y"; actionName = "Rotate"; }
            else if (mActiveGizmoElement == GizmoElement::Ring_Z) { elemName = "Ring Z"; actionName = "Rotate"; }

            if (isScaleGizmoElement(mActiveGizmoElement)) {
                snprintf(hudText, sizeof(hudText), " %dx%d  |  %s: [%s] Dragging (Ctrl: Uniform, Shift: Fine) ", (int) viewportSize.x, (int) viewportSize.y, actionName, elemName);
            } else {
                snprintf(hudText, sizeof(hudText), " %dx%d  |  %s: [%s] Dragging (Shift for precision) ", (int) viewportSize.x, (int) viewportSize.y, actionName, elemName);
            }
        } else {
            snprintf(hudText, sizeof(hudText), " %dx%d  |  Camera: RMB to Look + WASD, MMB to Pan ", (int) viewportSize.x, (int) viewportSize.y);
        }
        ImVec2 hudTextSize = ImGui::CalcTextSize(hudText);
        ImVec2 hudBgMax = ImVec2(hudPos.x + hudTextSize.x + 8.0f, hudPos.y + hudTextSize.y + 8.0f);

        drawList->AddRectFilled(hudPos, hudBgMax, IM_COL32(18, 19, 24, 200), 5.0f);
        drawList->AddRect(hudPos, hudBgMax, IM_COL32(50, 52, 65, 200), 5.0f);
        drawList->AddText(ImVec2(hudPos.x + 4.0f, hudPos.y + 4.0f), IM_COL32(200, 205, 220, 240), hudText);

        ImGui::End();
    }
}
