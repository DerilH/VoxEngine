#include "./InspectorComponent.h"

#include <imgui.h>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>
#include <VoxEngine/scene/components/MeshRendererComponent.h>
#include <VoxEngine/scene/components/Transform.h>
#include "../Theme.h"

namespace Vox::Editor {
    InspectorComponent::InspectorComponent(Ref<Engine> engine) : mEngine(engine) {
    }

    void InspectorComponent::setObject(Ref<Scene::GameObject> gameObject) {
        mGameObject = gameObject;
        if (mGameObject && mGameObject->transform) {
            mMeshRotation = glm::degrees(glm::eulerAngles(mGameObject->transform->getRotation()));
        }
    }

    void InspectorComponent::renderInternal(Render::RenderContext context) {
        ImGui::Begin("Inspector");

        if (!mGameObject) {
            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + avail.y * 0.35f);

            const char *msg1 = "No Object Selected";
            const char *msg2 = "Select an entity from Scene Hierarchy to inspect its properties.";

            float width1 = ImGui::CalcTextSize(msg1).x;
            if (avail.x > width1) {
                ImGui::SetCursorPosX((avail.x - width1) * 0.5f);
            }
            ImGui::TextColored(ImVec4(0.65f, 0.70f, 0.82f, 1.0f), "%s", msg1);

            ImGui::Spacing();
            float width2 = ImGui::CalcTextSize(msg2).x;
            if (avail.x > width2) {
                ImGui::SetCursorPosX((avail.x - width2) * 0.5f);
            }
            ImGui::TextColored(ImVec4(0.42f, 0.45f, 0.52f, 1.0f), "%s", msg2);
            ImGui::End();
            return;
        }

        // Header for selected GameObject
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::Accent);
        ImGui::Button("OBJ", ImVec2(36, 24));
        ImGui::PopStyleColor();
        ImGui::SameLine();

        char nameBuffer[128] = "";
        std::string currentName = mGameObject->getName();
        strncpy(nameBuffer, currentName.c_str(), sizeof(nameBuffer) - 1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##GameObjectName", nameBuffer, sizeof(nameBuffer))) {
            mGameObject->setName(InternedString(nameBuffer));
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        renderTransformOptions();
        renderComponents();

        ImGui::End();
    }

    void InspectorComponent::renderTransformOptions() {
        if (!mGameObject || !mGameObject->transform) return;

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Spacing();

            // Position
            auto pos = mGameObject->transform->getPos();
            if (DrawVec3Control("Position", pos, 0.0f, 75.0f, 0.05f)) {
                mGameObject->transform->setPos(pos);
            }

            ImGui::Spacing();

            // Rotation
            if (DrawVec3Control("Rotation", mMeshRotation, 0.0f, 75.0f, 1.0f)) {
                mGameObject->transform->setRotation(glm::quat(glm::radians(mMeshRotation)));
            }

            ImGui::Spacing();

            // Scale
            auto scale = mGameObject->transform->getScale();
            if (DrawVec3Control("Scale", scale, 1.0f, 75.0f, 0.05f)) {
                mGameObject->transform->setScale(scale);
            }

            ImGui::Spacing();
        }
    }

    void InspectorComponent::renderComponents() {
        if (!mGameObject) return;

        auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
        if (!renderer) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            float availW = ImGui::GetContentRegionAvail().x;
            float btnW = 160.0f;
            if (availW > btnW) {
                ImGui::SetCursorPosX((availW - btnW) * 0.5f);
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::Accent);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ThemeColors::AccentHover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ThemeColors::AccentActive);
            if (ImGui::Button("+ Add Component", ImVec2(btnW, 28))) {
                ImGui::OpenPopup("AddComponentPopup");
            }
            ImGui::PopStyleColor(3);

            if (ImGui::BeginPopup("AddComponentPopup")) {
                if (ImGui::MenuItem("Mesh Renderer")) {
                    auto comp = new Scene::MeshRendererComponent(mGameObject);
                    comp->setMaterial(mEngine->getResourceManager()->getDefault<Resources::MaterialAsset>());
                    comp->setMeshProvider([this]() {
                        auto model = mEngine->getResourceManager()->getDefault<Resources::ModelAsset>();
                        return static_cast<Ref<Resources::MeshAsset>>(model->getNested()[0]);
                    });
                    mGameObject->attachComponent(comp);
                }
                ImGui::EndPopup();
            }
            return;
        }

        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Spacing();

            bool changed = false;

            // Material Slot
            static char materialBuffer[512] = "";
            if (renderer->getMaterial()) {
                strncpy(materialBuffer, renderer->getMaterial()->getPath().c_str(), sizeof(materialBuffer) - 1);
            }

            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, 80.0f);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Material");
            ImGui::NextColumn();

            ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::AssetMaterial);
            ImGui::Button("MAT", ImVec2(36, 0));
            ImGui::PopStyleColor();
            ImGui::SameLine();

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::InputText("##Material", materialBuffer, IM_ARRAYSIZE(materialBuffer))) {
                changed = true;
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                    const char *draggedPath = static_cast<const char *>(payload->Data);
                    size_t dataSize = static_cast<size_t>(payload->DataSize);
                    size_t stringLen = (dataSize > 0 && draggedPath[dataSize - 1] == '\0') ? dataSize - 1 : dataSize;
                    if (stringLen >= sizeof(materialBuffer)) stringLen = sizeof(materialBuffer) - 1;
                    memcpy(materialBuffer, draggedPath, stringLen);
                    materialBuffer[stringLen] = '\0';
                    changed = true;
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::Columns(1);

            ImGui::Spacing();

            // Mesh Slot
            static char meshBuffer[512] = "";
            if (renderer->getMesh()) {
                strncpy(meshBuffer, renderer->getMesh()->getPath().c_str(), sizeof(meshBuffer) - 1);
            }

            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, 80.0f);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Mesh");
            ImGui::NextColumn();

            ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::AssetMesh);
            ImGui::Button("3D", ImVec2(36, 0));
            ImGui::PopStyleColor();
            ImGui::SameLine();

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::InputText("##Mesh", meshBuffer, IM_ARRAYSIZE(meshBuffer))) {
                changed = true;
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                    const char *draggedPath = static_cast<const char *>(payload->Data);
                    size_t dataSize = static_cast<size_t>(payload->DataSize);
                    size_t stringLen = (dataSize > 0 && draggedPath[dataSize - 1] == '\0') ? dataSize - 1 : dataSize;
                    if (stringLen >= sizeof(meshBuffer)) stringLen = sizeof(meshBuffer) - 1;
                    memcpy(meshBuffer, draggedPath, stringLen);
                    meshBuffer[stringLen] = '\0';
                    changed = true;
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::Columns(1);

            if (changed) {
                try {
                    auto newMat = mEngine->getResourceManager()->get<Resources::MaterialAsset>(InternedString(materialBuffer));
                    if (newMat) {
                        renderer->setMaterial(*newMat);
                    }
                    auto mesh = mEngine->getResourceManager()->get<Resources::MeshAsset>(InternedString(meshBuffer));
                    if (mesh.has_value()) {
                        renderer->setMeshProvider([mesh]() {
                            return *mesh;
                        });
                    }
                } catch (const std::exception &) {
                }
            }

            ImGui::Spacing();
        }
    }
}
