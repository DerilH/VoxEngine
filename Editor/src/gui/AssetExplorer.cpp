// Created by deril on 10/6/26.

#include "AssetExplorer.h"

#include <imgui.h>
#include <VoxEngine/resources/ResourcesManager.h>

#include "AssetIcon.h"
#include "assets/editor/MaterialEditor.h"
#include <VoxEngine/resources/assets/MaterialAsset.h>

namespace Vox::Editor {

    void AssetExplorer::render() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        if (mAssetEditor) {
            if (mShouldCloseEditor) {
                mShouldCloseEditor = false;
                mAssetEditor = nullptr;
            }
            else mAssetEditor->render();
        }

        ImGui::SetNextWindowPos(ImVec2(0, viewport->WorkSize.y - 200), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 200), ImGuiCond_Always);
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        ImGui::Begin("AssetExplorer", nullptr, windowFlags);

        ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable
                                   | ImGuiTableFlags_BordersInnerV
                                   | ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable("AssetExplorerTable", 2, tableFlags, ImGui::GetContentRegionAvail())) {

            ImGui::TableSetupColumn("TreeColumn", ImGuiTableColumnFlags_WidthFixed, 250.0f);
            ImGui::TableSetupColumn("ContentColumn", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);

            ImGuiWindowFlags treeChildFlags = ImGuiWindowFlags_HorizontalScrollbar
                                            | ImGuiWindowFlags_AlwaysVerticalScrollbar;

            if (ImGui::BeginChild("LeftTreePanel", ImVec2(-1.0f, 0.0f), false, treeChildFlags)) {
                renderTree();
            }
            ImGui::EndChild();

            ImGui::TableSetColumnIndex(1);

            if (ImGui::BeginChild("RightContentPanel", ImVec2(-1.0f, 0.0f), false)) {
                renderContent();
            }
            ImGui::EndChild();

            ImGui::EndTable();
        }

        ImGui::End();
    }

    void AssetExplorer::renderTree() {
        auto rootDir = Resources::ResourcesManager::Get().getRootDir();
        if (!rootDir) return;
        renderDirNode(rootDir);
    }

    void AssetExplorer::renderDirNode(ConstRef<Resources::AssetDirectory> dirNode) {
        if (!dirNode) return;

        ImGui::PushID(dirNode->path.c_str());

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                                  | ImGuiTreeNodeFlags_OpenOnDoubleClick
                                  | ImGuiTreeNodeFlags_SpanFullWidth;

        const auto& nestedDirs = dirNode->getNested();
        if (nestedDirs.empty()) {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        const std::string folderName = dirNode->name;

        if (mCurrentDir == dirNode) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        const bool isOpen = ImGui::TreeNodeEx(folderName.c_str(), flags);

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
            mCurrentDir = const_cast<Resources::AssetDirectory*>(dirNode);
        }

        if (isOpen) {
            for (const auto &childDir : nestedDirs) {
                renderDirNode(childDir.get());
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void AssetExplorer::renderContent() {
        if (!mCurrentDir) return;

        const auto& assets = Resources::ResourcesManager::Get().listDirAssets(mCurrentDir->path);
        if (assets.empty()) return;

        float cellSize = 74.0f;
        float availWidth = ImGui::GetContentRegionAvail().x;

        int columnCount = static_cast<int>(availWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable("AssetGridTable", columnCount, tableFlags)) {

            for (int i = 0; i < columnCount; ++i) {
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, cellSize);
            }

            for (auto& asset : assets) {
                ImGui::TableNextColumn();

                AssetIcon icon(asset);

                icon.setOnDoubleClick([asset, this, icon]() {
                    if (asset->type() == AssetType::MATERIAL) {
                        mAssetEditor = makeUPtr<MaterialEditor>(static_cast<ConstRef<Resources::MaterialAsset>>(asset), mShouldCloseEditor);
                    }
                });
                icon.render();
            }

            ImGui::EndTable();
        }
    }
}
