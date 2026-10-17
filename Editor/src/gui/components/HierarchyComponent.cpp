#include "./HierarchyComponent.h"
#include <imgui.h>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/GameObjectStorage.h>
#include <VoxEngine/scene/Scene.h>
#include <VoxEngine/scene/components/MeshRendererComponent.h>
#include <VoxEngine/scene/components/Transform.h>
#include "../Theme.h"

namespace Vox::Editor {
    HierarchyComponent::HierarchyComponent(Ref<Engine> engine) : mEngine(engine) {
    }

    void HierarchyComponent::duplicateObject(Scene::GameObjectRef orig) {
        if (!orig || !mScene) return;

        std::string newName = orig->getName() + " (Copy)";
        auto clone = mScene->createObject(newName);
        clone->transform->setPos(orig->transform->getPos() + glm::vec3(0.5f, 0.0f, 0.5f));
        clone->transform->setRotation(orig->transform->getRotation());
        clone->transform->setScale(orig->transform->getScale());


        auto renderer = orig->findComponent<Scene::MeshRendererComponent>();
        if (renderer) {
            auto cloneRenderer = new Scene::MeshRendererComponent(clone);
            cloneRenderer->setMaterial(renderer->getMaterial());
            auto mesh = renderer->getMesh();
            if (mesh) {
                cloneRenderer->setMeshProvider([mesh]() {
                    return mesh;
                });
            }
            clone->attachComponent(cloneRenderer);
        }

        mSelected = clone;
        onSelectedChanged.invoke(clone);
    }

    void HierarchyComponent::renderInternal(Render::RenderContext context) {
        if (!mScene) return;
        ImGui::Begin("Scene Hierarchy###Scene");

        // Keyboard shortcuts
        if (mSelected && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
            if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
                auto toDelete = mSelected;
                mSelected = nullptr;
                onSelectedChanged.invoke(nullptr);
                mScene->removeObject(toDelete);
            } else if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D)) {
                duplicateObject(mSelected);
            }
        }

        // Top Toolbar: Search + Add Button
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::Accent);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ThemeColors::AccentHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ThemeColors::AccentActive);
        if (ImGui::Button("+ Add")) {
            ImGui::OpenPopup("CreateObjectPopup");
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputTextWithHint("##HierarchyFilter", "Search entities...", mFilterBuffer, sizeof(mFilterBuffer));

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
        if (ImGui::BeginPopup("CreateObjectPopup")) {
            if (ImGui::MenuItem("Empty Object")) {
                mScene->createObject("Empty");
            }
            if (ImGui::BeginMenu("3D Object")) {
                if (ImGui::MenuItem("Cube")) {
                    auto cube = mScene->createObject("Cube");
                    auto renderer = new Scene::MeshRendererComponent(cube);
                    renderer->setMaterial(mEngine->getResourceManager()->getDefault<Resources::MaterialAsset>());
                    renderer->setMeshProvider([this]() {
                        auto model = mEngine->getResourceManager()->getDefault<Resources::ModelAsset>();
                        return static_cast<Ref<Resources::MeshAsset>>(model->getNested()[0]);
                    });
                    cube->attachComponent(renderer);
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Right-click in empty area
        if (ImGui::BeginPopupContextWindow("HierarchyEmptyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if (ImGui::MenuItem("Create Empty Object")) {
                mScene->createObject("Empty");
            }
            if (ImGui::MenuItem("Create 3D Cube")) {
                auto cube = mScene->createObject("Cube");
                auto renderer = new Scene::MeshRendererComponent(cube);
                renderer->setMaterial(mEngine->getResourceManager()->getDefault<Resources::MaterialAsset>());
                renderer->setMeshProvider([this]() {
                    auto model = mEngine->getResourceManager()->getDefault<Resources::ModelAsset>();
                    return static_cast<Ref<Resources::MeshAsset>>(model->getNested()[0]);
                });
                cube->attachComponent(renderer);
            }
            ImGui::EndPopup();
        }

        auto objs = mScene->getRootObjects();
        if (objs.empty()) {
            ImGui::Spacing();
            ImGui::TextDisabled("Scene is empty.");
            ImGui::TextDisabled("Right-click or click '+ Add' to create objects.");
        } else {
            for (const auto el: objs) {
                renderNode(el);
            }
        }

        ImGui::End();
    }

    void HierarchyComponent::renderNode(Scene::GameObjectRef el) {
        if (!el) return;

        std::string nameStr = el->getName();

        // Filter by search string if typed
        if (mFilterBuffer[0] != '\0') {
            if (nameStr.find(mFilterBuffer) == std::string::npos) {
                return;
            }
        }

        ImGui::PushID(el);

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                   ImGuiTreeNodeFlags_SpanAvailWidth;

        if (mSelected == el) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool hasChildren = (el->transform && el->transform->getChilds() && el->transform->getChilds()->count() > 0);
        if (!hasChildren) {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        bool hasMesh = (el->findComponent<Scene::MeshRendererComponent>() != nullptr);
        std::string label = (hasMesh ? "[3D] " : "[o] ") + nameStr;

        bool isOpen = ImGui::TreeNodeEx(el, flags, "%s", label.c_str());

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
            mSelected = el;
            onSelectedChanged.invoke(el);
        }

        // Context menu on the tree item
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Duplicate Object", "Ctrl+D")) {
                duplicateObject(el);
                ImGui::EndPopup();
                if (isOpen) ImGui::TreePop();
                ImGui::PopID();
                return;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete Object", "Del")) {
                if (mSelected == el) {
                    mSelected = nullptr;
                    onSelectedChanged.invoke(nullptr);
                }
                mScene->removeObject(el);
                ImGui::EndPopup();
                if (isOpen) ImGui::TreePop();
                ImGui::PopID();
                return;
            }
            ImGui::EndPopup();
        }

        if (isOpen) {
            if (hasChildren) {
                for (auto child: el->transform->getChilds()->getAll()) {
                    renderNode(child);
                }
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}
