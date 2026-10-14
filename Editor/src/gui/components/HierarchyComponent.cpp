#include "./HierarchyComponent.h"
#include <imgui.h>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/GameObjectStorage.h>
#include <VoxEngine/scene/Scene.h>
#include <VoxEngine/scene/components/MeshRendererComponent.h>
#include <VoxEngine/scene/components/Transform.h>


namespace Vox::Editor {
    void HierarchyComponent::renderInternal(Render::RenderContext context) {
        if (!mScene) return;
        ImGui::Begin("Scene");

        if (ImGui::Button("Create  v##CreateBtn")) {
            ImGui::OpenPopup("CreateObjectPopup");
        }
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
                if (ImGui::MenuItem("Sphere")){}
                ImGui::EndMenu();
            }
            // if (ImGui::BeginMenu("Light")) {
            //     if (ImGui::MenuItem("Directional")){}
            //     if (ImGui::MenuItem("Point")){}
            //     ImGui::EndMenu();
            // }
            // if (ImGui::MenuItem("Camera")) {
            // }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        auto objs = mScene->getRootObjects();
        for (const auto el: objs) {
            ImGui::PushID(el);

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick ;

            if (mSelected == el) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            if (el->transform->getChilds()->count() == 0) {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }

            const auto isOpen = ImGui::TreeNodeEx(el->getName().c_str(), flags);
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
                mSelected = el;
                onSelectedChanged.invoke(el);
            }

            if (isOpen) {
                ImGui::TreePop();
            }
            ImGui::PopID();
        }

        ImGui::End();
    }

    HierarchyComponent::HierarchyComponent(Ref<Engine> engine) : mEngine(engine) {
    }
}
