//
// Created by deril on 10/6/26.
//

#include "AssetIcon.h"

#include <imgui.h>

namespace Vox::Editor {

    AssetIcon::AssetIcon(const ConstRef<Resources::Asset> asset) : mAsset(asset) {
    }

    void AssetIcon::render() const {
        if (!mAsset) return;

        ImGui::PushID(mAsset);

        const auto &path = mAsset->getName();
        std::string fileName = path;

        auto iconSymbol = "[F]";

        ImGui::BeginGroup(); {
            ImVec2 iconSize(64.0f, 64.0f);

            if (ImGui::Button(iconSymbol, iconSize)) {
            }
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("ASSET_ITEM", path.c_str(), path.size() + 1);

                ImGui::Text("Move: %s", fileName.c_str());

                ImGui::EndDragDropSource();
            }
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + iconSize.x);
            ImGui::TextUnformatted(fileName.c_str());
            ImGui::PopTextWrapPos();
        }
        ImGui::EndGroup();
        ImGui::PopID();
    }
}
