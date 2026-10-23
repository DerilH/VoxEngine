// Created by deril on 10/6/26.

#include "AssetExplorer.h"

#include <imgui.h>
#include "AssetIcon.h"
#include "assets/editor/MaterialEditor.h"
#include "../assets/variants/MaterialAsset.h"
#include <VoxEngine/Engine.h>
#include "Gui.h"
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

        std::string currentPathStr = mCurrentDir ? mCurrentDir->path.c_str() : "Root";
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
        if (!mAssetManager) {
            mAssetManager = Gui::GetAssetManager();
        }
        if (!mAssetManager) return;
        auto rootDir = mAssetManager->getRootDir();
        if (!rootDir) return;
        renderDirNode(rootDir);
    }

    void AssetExplorer::renderDirNode(ConstRef<Assets::AssetDirectory> dirNode) {
        if (!dirNode) return;

        ImGui::PushID(dirNode->path.c_str());

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                                   | ImGuiTreeNodeFlags_OpenOnDoubleClick
                                   | ImGuiTreeNodeFlags_SpanAvailWidth;

        const auto &nestedDirs = dirNode->getNested();
        if (nestedDirs.empty()) {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        const std::string folderName = dirNode->name.c_str();

        if (mCurrentDir == dirNode) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        const bool isOpen = ImGui::TreeNodeEx(folderName.c_str(), flags);

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
            mCurrentDir = const_cast<Assets::AssetDirectory *>(dirNode);
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

        if (!mAssetManager) {
            mAssetManager = Gui::GetAssetManager();
        }
        if (!mAssetManager) return;

        for (const auto &uuid : mCurrentDir->getAssets()) {
            auto meta = mAssetManager->getMetadata(uuid);
            if (!meta) continue;

            auto icon = new AssetIcon(*meta);

            icon->setOnDoubleClick([uuid, type = meta->type, this]() {
                if (type == AssetType::MATERIAL) {
                    auto matAsset = mAssetManager->getOrLoad<Assets::MaterialAsset>(uuid);
                    if (matAsset) {
                        mAssetEditor = makeUPtr<MaterialEditor>(mEngine, uuid, *matAsset, mShouldCloseEditor);
                    }
                }
            });
            mIcons.emplace_back(icon);
        }
    }

    AssetExplorer::AssetExplorer(Ref<Engine> engine) : mEngine(engine), mAssetManager(Gui::GetAssetManager()) {
    }

    void AssetExplorer::openAsset(UUID uuid) {
        if (!uuid.isValid()) return;
        if (!mAssetManager) {
            mAssetManager = Gui::GetAssetManager();
        }
        if (!mAssetManager) return;
        auto meta = mAssetManager->getMetadata(uuid);
        if (!meta) return;

        auto path = mAssetManager->getPath(uuid);
        std::filesystem::path p(path.c_str());

        std::function<ConstRef<Assets::AssetDirectory>(ConstRef<Assets::AssetDirectory>, const std::string &)> findDir =
            [&](ConstRef<Assets::AssetDirectory> node, const std::string &dirPath) -> ConstRef<Assets::AssetDirectory> {
                if (!node) return nullptr;
                if (node->path == dirPath || (dirPath == "." && node->path == ".")) return node;
                for (const auto &child: node->getNested()) {
                    auto found = findDir(child.get(), dirPath);
                    if (found) return found;
                }
                return nullptr;
            };

        ConstRef<Assets::AssetDirectory> targetDir = nullptr;
        std::filesystem::path currentPath = p.parent_path();
        while (!targetDir && !currentPath.empty() && currentPath != ".") {
            targetDir = findDir(mAssetManager->getRootDir(), currentPath.string());
            currentPath = currentPath.parent_path();
        }
        if (!targetDir) {
            targetDir = findDir(mAssetManager->getRootDir(), ".");
        }

        if (targetDir) {
            mCurrentDir = targetDir;
            createContentIcons();

            for (auto &icon: mIcons) {
                if (icon->mAsset.uuid == uuid) {
                    icon->expanded = true;
                    break;
                }
            }
        }

        ImGui::SetWindowFocus("Asset Explorer###AssetExplorer");
    }

    void AssetExplorer::openAsset(InternedString path) {
        if (path.empty()) return;
        if (!mAssetManager) {
            mAssetManager = Gui::GetAssetManager();
        }
        if (!mAssetManager) return;

        UUID uuid = mAssetManager->getUUID(path);
        // if (!uuid.isValid()) uuid = UUID::fromString(path);
        // if (uuid.isValid()) {
            // openAsset(uuid);
        // }
    }
}
