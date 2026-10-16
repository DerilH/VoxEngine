#include "./InspectorComponent.h"

#include <imgui.h>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>
#include <VoxEngine/scene/components/MeshRendererComponent.h>
#include <VoxEngine/scene/components/Transform.h>

namespace Vox::Editor {
    void InspectorComponent::renderInternal(Render::RenderContext context) {
        ImGui::Begin("Inspector");
        if (mGameObject) {
            renderTransformOptions();
            renderComponents();
        }
        ImGui::End();
    }

    InspectorComponent::InspectorComponent(Ref<Engine> engine) : mEngine(engine) {
    }

    void InspectorComponent::setObject(Ref<Scene::GameObject> gameObject) {
        if (gameObject) {
            mGameObject = gameObject;
            mMeshRotation = glm::degrees(glm::eulerAngles(mGameObject->transform->getRotation()));
        }
    }

    void InspectorComponent::renderTransformOptions() {
        ImGui::Text("Tranform");
        if (auto el = mGameObject) {
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

    void InspectorComponent::renderComponents() {
        auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
        if (!renderer) return;

        auto mat = renderer->getMaterial()->cloneTyped();
        if (!mat) return;
        bool changed = false;
        if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            static char materialBuffer[512] = "";
            strncpy(materialBuffer, renderer->getMaterial()->getPath().c_str(), renderer->getMaterial()->getPath().size());
            if (ImGui::InputText("Material", materialBuffer, IM_ARRAYSIZE(materialBuffer))) {
                changed = true;
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                    const char *draggedPath = static_cast<const char *>(payload->Data);
                    size_t dataSize = static_cast<size_t>(payload->DataSize);
                    
                    size_t stringLen = dataSize;
                    if (stringLen > 0 && draggedPath[stringLen - 1] == '\0') {
                        stringLen--;
                    }

                    if (stringLen >= sizeof(materialBuffer)) {
                        stringLen = sizeof(materialBuffer) - 1;
                    }

                    memcpy(materialBuffer, draggedPath, stringLen);
                    materialBuffer[stringLen] = '\0';
                    changed = true;
                }
                ImGui::EndDragDropTarget();
            }

            static char meshBuffer[512] = "";
            strncpy(meshBuffer, renderer->getMesh()->getPath().c_str(), renderer->getMesh()->getPath().size());
            if (ImGui::InputText("Mesh", meshBuffer, IM_ARRAYSIZE(meshBuffer))) {
                changed = true;
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                    const char *draggedPath = static_cast<const char *>(payload->Data);
                    size_t dataSize = static_cast<size_t>(payload->DataSize);
                    
                    size_t stringLen = dataSize;
                    if (stringLen > 0 && draggedPath[stringLen - 1] == '\0') {
                        stringLen--;
                    }

                    if (stringLen >= sizeof(meshBuffer)) {
                        stringLen = sizeof(meshBuffer) - 1;
                    }

                    memcpy(meshBuffer, draggedPath, stringLen);
                    meshBuffer[stringLen] = '\0';
                    changed = true;
                }
                ImGui::EndDragDropTarget();
            }


            try {
                if (changed) {
                    auto newMat = mEngine->getResourceManager()->get<Resources::MaterialAsset>(InternedString(materialBuffer));

                    if (newMat) {
                        renderer->setMaterial(*newMat);
                        auto mesh = mEngine->getResourceManager()->get<Resources::MeshAsset>(InternedString(meshBuffer));
                        if (mesh.has_value()) {
                            renderer->setMeshProvider([mesh]() {
                                return *mesh;
                            });
                        }
                    }
                }
            } catch (std::exception &e) {
            }
        }
    }
}
