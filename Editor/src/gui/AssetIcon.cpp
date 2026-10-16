//
// Created by deril on 10/6/26.
//

#include "AssetIcon.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Vox::Editor {
    AssetIcon::AssetIcon(const ConstRef<Resources::Asset> asset) : mAsset(asset) {
        auto nested = mAsset->getNested();
        for (int i = 0; i < mAsset->getNestedCount(); i++) {
            auto icon = new AssetIcon(nested[i]);
            mNestedIcons.emplace_back(icon);
        }
    }

    void AssetIcon::render() const {
        if (!mAsset) return;

        ImGui::PushID(mAsset);

        std::string fileName = mAsset->getName();
        auto iconSymbol = "[F]";
        bool hasNested = mAsset->hasNested();

        ImGui::BeginGroup(); {
            ImVec2 iconSize(64.0f, 64.0f);
            ImVec2 iconStartPos = ImGui::GetCursorScreenPos();

            if (hasNested) {
                float arrowSize = 16.0f;

                ImVec2 arrowPos = ImVec2(
                    iconStartPos.x + iconSize.x - arrowSize - 2.0f,
                    iconStartPos.y + (iconSize.y - arrowSize) * 0.5f
                );

                ImVec2 currentCursorPos = ImGui::GetCursorPos();

                ImGui::SetCursorScreenPos(arrowPos);

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0.4f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.4f, 0.7f, 1.0f));

                ImGuiDir arrowDir = expanded ? ImGuiDir_Down : ImGuiDir_Right;
                if (ImGui::ArrowButton("##ExpandArrow", arrowDir)) {
                    expanded = !expanded;
                }

                ImGui::PopStyleColor(3);
                ImGui::SetCursorPos(currentCursorPos);
            }

            if (ImGui::Button(iconSymbol, iconSize)) {
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (mDoubleClickCallback) {
                    mDoubleClickCallback();
                }
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("ASSET_ITEM", mAsset->getPath().c_str(), mAsset->getPath().size());
                ImGui::Text("Move: %s", fileName.c_str());
                ImGui::EndDragDropSource();
            }

            ImVec2 textPos = ImGui::GetCursorScreenPos();
            ImVec2 textSize = ImGui::CalcTextSize(fileName.c_str());

            float maxTextHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f;
            ImVec2 clipMax = ImVec2(textPos.x + iconSize.x, textPos.y + maxTextHeight);

            ImFont* font = ImGui::GetFont();
            float oldScale = font->Scale;
            font->Scale = 0.5f;
            ImGui::PushFont(font);

            ImGui::RenderTextClipped(
                textPos,
                clipMax,
                fileName.c_str(),
                nullptr,
                &textSize,
                ImVec2(0.5f, 0.0f)
            );
            ImGui::PopFont();
            font->Scale = oldScale;
            float renderedHeight = std::min(textSize.y, maxTextHeight);
            ImGui::Dummy(ImVec2(iconSize.x, renderedHeight));

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", fileName.c_str());
            }
        }
        ImGui::EndGroup();
        ImGui::PopID();
    }
}
