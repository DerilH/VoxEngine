//
// Created by deril on 10/6/26.
//

#include "AssetIcon.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "Theme.h"

EDITOR_NS
    AssetIcon::AssetIcon(const Assets::AssetMetadata asset) : mAsset(asset) {
        // auto nested = mAsset.getNested();
        // for (int i = 0; i < mAsset.getNestedCount(); i++) {
            // auto icon = new AssetIcon(nested[i]);
            // mNestedIcons.emplace_back(icon);
        // }
    }

    void AssetIcon::render() const {
        ImGui::PushID(1);

        // bool hasNested = mAsset->hasNested();
        bool hasNested = false;
        AssetType type = mAsset.type;
        const char *badge = GetAssetTypeBadge(type);
        ImVec4 badgeColor = GetAssetTypeColor(type);

        const float cardWidth = 76.0f;
        const float cardHeight = 84.0f;
        const float thumbHeight = 56.0f;

        ImVec2 startPos = ImGui::GetCursorScreenPos();
        ImDrawList *drawList = ImGui::GetWindowDrawList();

        // Invisible button to capture all mouse events over the card
        ImGui::InvisibleButton("##AssetCardBtn", ImVec2(cardWidth, cardHeight));
        bool isHovered = ImGui::IsItemHovered();
        bool isActive = ImGui::IsItemActive();

        if (isHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (mDoubleClickCallback) {
                mDoubleClickCallback();
            }
        }

        // Drag and drop source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("ASSET_ITEM", &mAsset.uuid, sizeof(mAsset.uuid));
            ImGui::PushStyleColor(ImGuiCol_Text, badgeColor);
            ImGui::Text("[%s]", badge);
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::EndDragDropSource();
        }

        // Draw card background
        ImU32 bgCol = isHovered ? (isActive ? IM_COL32(45, 48, 62, 255) : IM_COL32(36, 38, 48, 255))
                                : IM_COL32(24, 25, 31, 255);
        ImU32 borderCol = isHovered ? IM_COL32(99, 128, 242, 230) : IM_COL32(50, 52, 64, 180);

        // Thumbnail background & border
        drawList->AddRectFilled(startPos, ImVec2(startPos.x + cardWidth, startPos.y + thumbHeight), bgCol, 5.0f);
        drawList->AddRect(startPos, ImVec2(startPos.x + cardWidth, startPos.y + thumbHeight), borderCol, 5.0f);

        // Badge pill in center of thumbnail
        ImVec2 badgeSize = ImGui::CalcTextSize(badge);
        float pillW = badgeSize.x + 12.0f;
        float pillH = badgeSize.y + 6.0f;
        ImVec2 pillPos = ImVec2(startPos.x + (cardWidth - pillW) * 0.5f, startPos.y + (thumbHeight - pillH) * 0.5f);

        ImU32 badgeColU32 = ImGui::ColorConvertFloat4ToU32(badgeColor);
        // Pill background (translucent tint)
        ImVec4 pillBgVec = badgeColor;
        pillBgVec.w = 0.22f;
        drawList->AddRectFilled(pillPos, ImVec2(pillPos.x + pillW, pillPos.y + pillH), ImGui::ColorConvertFloat4ToU32(pillBgVec), 4.0f);
        drawList->AddRect(pillPos, ImVec2(pillPos.x + pillW, pillPos.y + pillH), badgeColU32, 4.0f);

        // Badge text
        drawList->AddText(ImVec2(pillPos.x + 6.0f, pillPos.y + 3.0f), badgeColU32, badge);

        // Expand arrow if has nested
        if (hasNested) {
            float arrowSize = 14.0f;
            ImVec2 arrowPos = ImVec2(startPos.x + cardWidth - arrowSize - 2.0f, startPos.y + 2.0f);
            ImGui::SetCursorScreenPos(arrowPos);
            ImGuiDir arrowDir = expanded ? ImGuiDir_Down : ImGuiDir_Right;
            if (ImGui::ArrowButton("##ExpandArrow", arrowDir)) {
                expanded = !expanded;
            }
        }

        // File label below thumbnail
        ImVec2 textPos = ImVec2(startPos.x + 2.0f, startPos.y + thumbHeight + 4.0f);
        ImVec2 clipMax = ImVec2(startPos.x + cardWidth - 2.0f, startPos.y + cardHeight);
        ImVec2 textSize = ImGui::CalcTextSize(mAsset.fileName.c_str());

        ImGui::RenderTextClipped(
            textPos,
            clipMax,
            mAsset.fileName.c_str(),
            nullptr,
            &textSize,
            ImVec2(0.5f, 0.0f)
        );

        if (isHovered) {
            ImGui::BeginTooltip();
            ImGui::TextColored(badgeColor, "[%s]", badge);
            ImGui::SameLine();
            ImGui::Text("%s", mAsset.fileName.c_str());
            ImGui::EndTooltip();
        }

        ImGui::PopID();
    }
NS_END