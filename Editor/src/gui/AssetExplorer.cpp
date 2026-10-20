// Created by deril on 10/6/26.

#include "AssetExplorer.h"

#include <imgui.h>
#include "AssetIcon.h"
#include "assets/editor/MaterialEditor.h"
#include <../assets/MaterialAsset.h>
#include <VoxEngine/Engine.h>
#include "Theme.h"

namespace Vox::Editor {
    void AssetExplorer::renderInternal(Render::RenderContext context) {
        if (mAssetEditor) {
            if (mShouldCloseEditor) {
                mShouldCloseEditor = false;
                mAssetEditor = nullptr;
            } else {
                mAssetEditor->render(context);
            }
        }

        ImGui::Begin("Asset Explorer###AssetExplorer", nullptr);

        // Top Toolbar / Breadcrumb Bar
        ImGui::Spacing();
        DrawBadge("DIR", ThemeColors::AssetDir);
        ImGui::SameLine();

        std::string currentPathStr = mCurrentDir ? mCurrentDir->path : "Root";
        ImGui::Text("Resources / %s", currentPathStr.c_str());

        ImGui::SameLine();
        float availX = ImGui::GetContentRegionAvail().x;
        float counterWidth = 80.0f;
        if (availX > counterWidth) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availX - counterWidth);
            ImGui::TextDisabled("%zu items", mIcons.size());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable
                                     | ImGuiTableFlags_BordersInnerV
                                     | ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable("AssetExplorerTable", 2, tableFlags, ImGui::GetContentRegionAvail())) {
            ImGui::TableSetupColumn("TreeColumn", ImGuiTableColumnFlags_WidthFixed, 220.0f);
            ImGui::TableSetupColumn("ContentColumn", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();

            // Left Tree Column
            ImGui::TableSetColumnIndex(0);
            ImGuiWindowFlags treeChildFlags = ImGuiWindowFlags_HorizontalScrollbar
                                              | ImGuiWindowFlags_AlwaysVerticalScrollbar;

            if (ImGui::BeginChild("LeftTreePanel", ImVec2(-1.0f, 0.0f), false, treeChildFlags)) {
                renderTree();
            }
            ImGui::EndChild();

            // Right Content Column
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
        auto rootDir = mResManager->getRootDir();
        if (!rootDir) return;
        renderDirNode(rootDir);
    }

    void AssetExplorer::renderDirNode(ConstRef<Resources::AssetDirectory> dirNode) {
        if (!dirNode) return;

        ImGui::PushID(dirNode->path.c_str());

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                                   | ImGuiTreeNodeFlags_OpenOnDoubleClick
                                   | ImGuiTreeNodeFlags_SpanAvailWidth;

        const auto &nestedDirs = dirNode->getNested();
        if (nestedDirs.empty()) {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        const std::string folderName = dirNode->name;

        if (mCurrentDir == dirNode) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        const bool isOpen = ImGui::TreeNodeEx(folderName.c_str(), flags);

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
            mCurrentDir = const_cast<Resources::AssetDirectory *>(dirNode);
            createContentIcons();
        }

        if (isOpen) {
            for (const auto &childDir: nestedDirs) {
                renderDirNode(childDir.get());
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void AssetExplorer::renderContent() {
        if (!mCurrentDir) {
            ImGui::Spacing();
            ImGui::TextDisabled("Select a folder in the tree to view assets.");
            return;
        }

        if (mIcons.empty()) {
            ImGui::Spacing();
            ImGui::TextDisabled("This folder is empty.");
            return;
        }

        float cellSize = 88.0f;
        float availWidth = ImGui::GetContentRegionAvail().x;

        int columnCount = static_cast<int>(availWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings;

        if (ImGui::BeginTable("AssetGridTable", columnCount, tableFlags)) {
            for (int i = 0; i < columnCount; ++i) {
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, cellSize);
            }

            auto renderIconItem = [&](auto &iconPtr) {
                ImGui::TableNextColumn();
                iconPtr->render();
            };

            for (auto &icon : mIcons) {
                renderIconItem(icon);

                if (icon->expanded) {
                    for (auto &nested : icon->getNested()) {
                        renderIconItem(nested);
                    }
                }
            }

            ImGui::EndTable();
        }
    }

    void AssetExplorer::createContentIcons() {
        if (!mCurrentDir) return;
        mIcons.clear();

        const auto &assets = mResManager->listDirAssets(mCurrentDir->path);
        if (assets.empty()) return;

        for (auto &asset: assets) {
            auto icon = new AssetIcon(asset);

            icon->setOnDoubleClick([asset, this, icon]() {
                if (asset->type() == AssetType::MATERIAL) {
                    mAssetEditor = makeUPtr<MaterialEditor>(mEngine, static_cast<ConstRef<Resources::MaterialAsset>>(asset), mShouldCloseEditor);
                }
            });
            mIcons.emplace_back(icon);
        }
    }

    AssetExplorer::AssetExplorer(Ref<Engine> engine) : mEngine(engine), mResManager(engine->getResourceManager()) {
    }

    void AssetExplorer::openAsset(InternedString path) {
        if (path.empty()) return;
        auto asset = mResManager->getAsset(path);
        if (!asset) return;

        std::filesystem::path p(path.c_str());

        std::function<ConstRef<Resources::AssetDirectory>(ConstRef<Resources::AssetDirectory>, const std::string &)> findDir =
            [&](ConstRef<Resources::AssetDirectory> node, const std::string &dirPath) -> ConstRef<Resources::AssetDirectory> {
                if (!node) return nullptr;
                if (node->path == dirPath || (dirPath == "." && node->path == ".")) return node;
                for (const auto &child: node->getNested()) {
                    auto found = findDir(child.get(), dirPath);
                    if (found) return found;
                }
                return nullptr;
            };

        ConstRef<Resources::AssetDirectory> targetDir = nullptr;
        std::filesystem::path currentPath = p.parent_path();
        while (!targetDir && !currentPath.empty() && currentPath != ".") {
            targetDir = findDir(mResManager->getRootDir(), currentPath.string());
            currentPath = currentPath.parent_path();
        }
        if (!targetDir) {
            targetDir = findDir(mResManager->getRootDir(), ".");
        }

        if (targetDir) {
            mCurrentDir = targetDir;
            createContentIcons();

            for (auto &icon: mIcons) {
                for (auto &nested: icon->getNested()) {
                    if (nested->mAsset && nested->mAsset->getPath() == path) {
                        icon->expanded = true;
                        break;
                    }
                }
            }
        }

        // if (asset->type() == AssetType::MATERIAL) {
            // mAssetEditor = makeUPtr<MaterialEditor>(mEngine, static_cast<ConstRef<Resources::MaterialAsset>>(asset), mShouldCloseEditor);
        // }

        ImGui::SetWindowFocus("Asset Explorer###AssetExplorer");
    }

    void AssetExplorer::openAsset(ConstRef<Resources::Asset> asset) {
        if (!asset) return;
        openAsset(asset->getPath());
    }
}
