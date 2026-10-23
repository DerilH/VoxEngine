#include "./InspectorComponent.h"

#include <imgui.h>
#include "../../assets/AssetConverter.h"
#include "../../assets/AssetManager.h"
#include "../../assets/variants/MaterialAsset.h"
#include "../../assets/variants/MeshAsset.h"
#include "../../assets/variants/ModelAsset.h"
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
            if (renderer && renderer->getMaterialUuid().isValid()) {
                auto assetMgr = Gui::GetAssetManager();
                if (assetMgr) {
                    auto meta = assetMgr->getMetadata(renderer->getMaterialUuid());
                    if (meta) return meta->fileName;
                }
            }
            return "";
        });

        auto applyMaterial = [this](UUID matUuid) {
            if (!mGameObject || !matUuid.isValid()) return;
            auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
            if (!renderer) return;
            auto assetMgr = Gui::GetAssetManager();
            if (!assetMgr) return;
            auto resMgr = mEngine->getResourceManager();
            auto matRes = resMgr->findByAsset<Resources::MaterialResource>(matUuid);
            if (!matRes) {
                auto newMat = assetMgr->getOrLoad<Assets::MaterialAsset>(matUuid);
                if (newMat) {
                    auto handle = Assets::AssetConverter::toResource(*newMat, resMgr, assetMgr, matUuid);
                    if (handle) matRes = *handle;
                }
            }
            if (matRes) {
                auto r = resMgr->get(*matRes);
                if (r) renderer->setMaterial(r.value(), matUuid);
            }
        };

        mMaterialLink->onAssetChanged += applyMaterial;
        mMaterialLink->onChanged += [this, applyMaterial](UUID pathOrUuid) {
            auto assetMgr = Gui::GetAssetManager();
            if (!assetMgr) return;
            // if (!uuid.isValid()) uuid = UUID::fromString(pathOrUuid);
            if (pathOrUuid.isValid()) applyMaterial(pathOrUuid);
        };

        mMaterialLink->onAssetDoubleClicked += [this](UUID uuid) {
            if (uuid.isValid()) {
                onOpenAssetUuid.invoke(uuid);
                auto assetMgr = Gui::GetAssetManager();
                if (assetMgr) {
                    auto path = assetMgr->getPath(uuid);
                    if (!path.empty()) onOpenAsset.invoke(path);
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
            if (renderer && renderer->getMaterialUuid().isValid()) {
                auto assetMgr = Gui::GetAssetManager();
                if (assetMgr) {
                    auto matAsset = assetMgr->getOrLoad<Assets::MaterialAsset>(renderer->getMaterialUuid());
                    if (matAsset && matAsset.value()->albedo.isValid()) {
                        auto meta = assetMgr->getMetadata(matAsset.value()->albedo);
                        if (meta) return meta->fileName;
                    }
                }
            }
            return "";
        });

        auto applyTexture = [this](UUID texUuid) {
            if (!mGameObject || !texUuid.isValid()) return;
            auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
            if (!renderer || !renderer->getMaterialUuid().isValid()) return;

            auto assetMgr = Gui::GetAssetManager();
            if (!assetMgr) return;
            auto matUuid = renderer->getMaterialUuid();
            auto oldMat = assetMgr->getForEdit<Assets::MaterialAsset>(matUuid);
            if (oldMat) {
                oldMat->albedo = texUuid;
                assetMgr->update(matUuid, std::move(oldMat));
                auto updated = assetMgr->getOrLoad<Assets::MaterialAsset>(matUuid);
                if (updated) {
                    auto resMgr = mEngine->getResourceManager();
                    auto handle = Assets::AssetConverter::toResource(*updated, resMgr, assetMgr, matUuid);
                    if (handle) {
                        auto matRes = resMgr->get(*handle);
                        if (matRes) renderer->setMaterial(matRes.value(), matUuid);
                    }
                }
            }
        };

        mTextureLink->onAssetChanged += applyTexture;
        mTextureLink->onChanged += [this, applyTexture](UUID uuid) {
            auto assetMgr = Gui::GetAssetManager();
            if (!assetMgr) return;
            if (uuid.isValid()) applyTexture(uuid);
        };

        mTextureLink->onAssetDoubleClicked += [this](UUID uuid) {
            if (uuid.isValid()) {
                onOpenAssetUuid.invoke(uuid);
                auto assetMgr = Gui::GetAssetManager();
                if (assetMgr) {
                    auto path = assetMgr->getPath(uuid);
                    if (!path.empty()) onOpenAsset.invoke(path);
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
            if (renderer && renderer->getMeshUuid().isValid()) {
                auto assetMgr = Gui::GetAssetManager();
                if (assetMgr) {
                    auto meta = assetMgr->getMetadata(renderer->getMeshUuid());
                    if (meta) return meta->fileName;
                }
            }
            return "";
        });

        auto applyMesh = [this](UUID meshUuid) {
            if (!mGameObject || !meshUuid.isValid()) return;
            auto renderer = mGameObject->findComponent<Scene::MeshRendererComponent>();
            if (!renderer) return;
            auto assetMgr = Gui::GetAssetManager();
            if (!assetMgr) return;
            auto resMgr = mEngine->getResourceManager();

            auto meta = assetMgr->getMetadata(meshUuid);
            if (!meta) return;

            if (meta->type == AssetType::MODEL) {
                auto modelRes = resMgr->findByAsset<Resources::ModelResource>(meshUuid);
                if (!modelRes) {
                    auto modelAsset = assetMgr->getOrLoad<Assets::ModelAsset>(meshUuid);
                    if (modelAsset) {
                        auto handle = Assets::AssetConverter::toResource(*modelAsset, resMgr, meshUuid);
                        if (handle) modelRes = *handle;
                    }
                }
                if (modelRes) {
                    auto model = resMgr->get(*modelRes);
                    if (model && model.value()->getMeshCount() > 0) {
                        auto meshHandle = model.value()->getMesh(0);
                        renderer->setMeshProvider([resMgr, meshHandle]() -> ConstRef<Resources::MeshResource> {
                            auto m = resMgr->get(meshHandle);
                            return m ? m.value() : nullptr;
                        }, meshUuid);
                    }
                }
            } else if (meta->type == AssetType::MESH) {
                auto meshRes = resMgr->findByAsset<Resources::MeshResource>(meshUuid);
                if (!meshRes) {
                    auto meshAsset = assetMgr->getOrLoad<Assets::MeshAsset>(meshUuid);
                    if (meshAsset) {
                        auto handle = Assets::AssetConverter::toResource(*meshAsset, resMgr, meshUuid);
                        if (handle) meshRes = *handle;
                    }
                }
                if (meshRes) {
                    auto h = *meshRes;
                    renderer->setMeshProvider([resMgr, h]() -> ConstRef<Resources::MeshResource> {
                        auto m = resMgr->get(h);
                        return m ? m.value() : nullptr;
                    }, meshUuid);
                }
            }
        };

        mMeshLink->onAssetChanged += applyMesh;
        mMeshLink->onChanged += [this, applyMesh](UUID uuid) {
            auto assetMgr = Gui::GetAssetManager();
            if (!assetMgr) return;
            if (uuid.isValid()) applyMesh(uuid);
        };

        mMeshLink->onAssetDoubleClicked += [this](UUID uuid) {
            if (uuid.isValid()) {
                onOpenAssetUuid.invoke(uuid);
                auto assetMgr = Gui::GetAssetManager();
                if (assetMgr) {
                    auto path = assetMgr->getPath(uuid);
                    if (!path.empty()) onOpenAsset.invoke(path);
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
                    auto assetMgr = Gui::GetAssetManager();
                    UUID matUuid = assetMgr ? assetMgr->getUUID("defaults/DefaultMaterial.vmat") : UUID{};
                    UUID cubeUuid = assetMgr ? assetMgr->getUUID("defaults/DefaultCube.fbx") : UUID{};
                    if (matUuid.isValid()) {
                        auto matHandle = resMgr->findByAsset<Resources::MaterialResource>(matUuid);
                        if (matHandle) {
                            auto matRes = resMgr->get(*matHandle);
                            if (matRes) comp->setMaterial(matRes.value(), matUuid);
                        }
                    }
                    if (cubeUuid.isValid()) {
                        auto modelHandle = resMgr->findByAsset<Resources::ModelResource>(cubeUuid);
                        if (modelHandle) {
                            auto model = resMgr->get(*modelHandle);
                            if (model && model.value()->getMeshCount() > 0) {
                                auto meshHandle = model.value()->getMesh(0);
                                comp->setMeshProvider([resMgr, meshHandle]() -> ConstRef<Resources::MeshResource> {
                                    auto m = resMgr->get(meshHandle);
                                    return m ? m.value() : nullptr;
                                }, cubeUuid);
                            }
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
