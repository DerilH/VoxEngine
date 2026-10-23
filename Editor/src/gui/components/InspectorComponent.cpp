#include "./InspectorComponent.h"

#include <imgui.h>
#include "../../assets/AssetConverter.h"
#include "../../assets/AssetManager.h"
#include "../../assets/variants/MaterialAsset.h"
#include "../../assets/variants/MeshAsset.h"
#include "../Gui.h"
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/resources/MaterialResource.h>
#include <VoxEngine/resources/ModelResource.h>
#include <VoxEngine/resources/MeshResource.h>
#include <VoxEngine/resources/TextureResource.h>
#include <VoxEngine/scene/components/MeshRendererComponent.h>
#include <VoxEngine/scene/components/Transform.h>
#include "../Theme.h"

namespace Vox::Editor {
    InspectorComponent::InspectorComponent(Ref<Engine> engine) : mEngine(engine) {
        mMaterialLink = makeUPtr<AssetLinkComponent>("Material", AssetType::MATERIAL, [this]() -> InternedString {
            if (!mGameObject) return "";
            auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
            if (renderer && renderer->getMaterial()) {
                return renderer->getMaterial()->getName();
            }
            return "";
        });

        mMaterialLink->onChanged += [this](InternedString path) {
            if (!mGameObject) return;
            auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
            if (!renderer) return;
            auto assetMgr = Gui::GetAssetManager();
            if (!assetMgr) return;
            auto newMat = assetMgr->findByPath<Assets::MaterialAsset>(path);
            if (newMat) {
                auto resMgr = mEngine->getResourceManager();
                auto matHandle = Assets::AssetConverter::toResource(*newMat, resMgr, assetMgr);
                if (matHandle) {
                    auto matRes = resMgr->get(*matHandle);
                    if (matRes) renderer->setMaterial(matRes.value());
                }
            }
        };

        mMaterialLink->onDoubleClicked += [this](InternedString path) {
            if (!path.empty()) {
                onOpenAsset.invoke(path);
            }
        };

        mTextureLink = makeUPtr<AssetLinkComponent>("Texture", AssetType::TEXTURE, [this]() -> InternedString {
            if (!mGameObject) return "";
            auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
            if (renderer && renderer->getMaterial()) {
                auto texHandle = renderer->getMaterial()->getTexture();
                if (texHandle.isValid()) {
                    auto resMgr = mEngine->getResourceManager();
                    auto texRes = resMgr->get(texHandle);
                    if (texRes) return texRes.value()->getName();
                }
            }
            return "";
        });

        mTextureLink->onChanged += [this](InternedString path) {
            if (!mGameObject) return;
            auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
            if (!renderer || !renderer->getMaterial()) return;

            auto assetMgr = Gui::GetAssetManager();
            if (!assetMgr) return;
            auto matPath = renderer->getMaterial()->getName();
            auto oldMat = assetMgr->findByPath<Assets::MaterialAsset>(matPath);
            if (oldMat) {
                auto newMat = (*oldMat)->cloneTyped();
                newMat->texture = path;
                newMat->albedo = path;
                assetMgr->update(matPath, std::move(newMat));
                auto updated = assetMgr->findByPath<Assets::MaterialAsset>(matPath);
                if (updated) {
                    auto resMgr = mEngine->getResourceManager();
                    auto handle = Assets::AssetConverter::toResource(*updated, resMgr, assetMgr);
                    if (handle) {
                        auto matRes = resMgr->get(*handle);
                        if (matRes) renderer->setMaterial(matRes.value());
                    }
                }
            }
        };

        mTextureLink->onDoubleClicked += [this](InternedString path) {
            if (!path.empty()) {
                onOpenAsset.invoke(path);
            }
        };

        mMeshLink = makeUPtr<AssetLinkComponent>("Mesh", AssetType::MESH, [this]() -> InternedString {
            if (!mGameObject) return "";
            auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
            if (renderer && renderer->getMesh()) {
                return renderer->getMesh()->getName();
            }
            return "";
        });

        mMeshLink->onChanged += [this](InternedString path) {
            if (!mGameObject) return;
            auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
            if (!renderer) return;
            auto assetMgr = Gui::GetAssetManager();
            if (!assetMgr) return;
            auto mesh = assetMgr->findByPath<Assets::MeshAsset>(path);
            if (mesh) {
                auto resMgr = mEngine->getResourceManager();
                auto meshHandle = Assets::AssetConverter::toResource(*mesh, resMgr);
                if (meshHandle) {
                    auto h = *meshHandle;
                    renderer->setMeshProvider([resMgr, h]() -> ConstRef<Resources::MeshResource> {
                        auto m = resMgr->get(h);
                        return m ? m.value() : nullptr;
                    });
                }
            }
        };

        mMeshLink->onDoubleClicked += [this](InternedString path) {
            if (!path.empty()) {
                onOpenAsset.invoke(path);
            }
        };
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
        DrawBadge("OBJ", ThemeColors::Accent);
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
        renderComponents(context);

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

    void InspectorComponent::renderComponents(Render::RenderContext context) {
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
                    auto resMgr = mEngine->getResourceManager();
                    auto matHandle = resMgr->find<Resources::MaterialResource>("defaults/DefaultMaterial.vmat");
                    if (matHandle) {
                        auto matRes = resMgr->get(*matHandle);
                        if (matRes) comp->setMaterial(matRes.value());
                    }
                    auto modelHandle = resMgr->find<Resources::ModelResource>("defaults/DefaultCube.fbx");
                    if (modelHandle) {
                        auto model = resMgr->get(*modelHandle);
                        if (model && model.value()->getMeshCount() > 0) {
                            auto meshHandle = model.value()->getMesh(0);
                            comp->setMeshProvider([resMgr, meshHandle]() -> ConstRef<Resources::MeshResource> {
                                auto m = resMgr->get(meshHandle);
                                return m ? m.value() : nullptr;
                            });
                        }
                    }
                    mGameObject->attachComponent(comp);
                }
                ImGui::EndPopup();
            }
            return;
        }

        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Spacing();

            // Layer Slot
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, 80.0f);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Layer");
            ImGui::NextColumn();

            auto currentLayer = renderer->getLayer();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (RenderEnumCombo("##Layer", currentLayer)) {
                renderer->setLayer(currentLayer);
            }
            ImGui::Columns(1);

            ImGui::Spacing();

            // Material Slot (via AssetLinkComponent)
            mMaterialLink->render(context);

            ImGui::Spacing();

            // Texture Slot (via AssetLinkComponent)
            if (renderer->getMaterial()) {
                mTextureLink->render(context);
                ImGui::Spacing();
            }

            // Mesh Slot (via AssetLinkComponent)
            mMeshLink->render(context);

            ImGui::Spacing();
        }
    }
}
