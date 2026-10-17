#include "Theme.h"
#include <imgui_internal.h>
#include <vector>

namespace Vox::Editor {

    void ApplyVoxTheme() {
        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors = style.Colors;

        // Visual Geometry & Ergonomics
        style.WindowRounding    = 6.0f;
        style.ChildRounding     = 5.0f;
        style.FrameRounding     = 5.0f;
        style.PopupRounding     = 6.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabRounding      = 4.0f;
        style.TabRounding       = 5.0f;

        style.WindowBorderSize  = 1.0f;
        style.ChildBorderSize   = 1.0f;
        style.PopupBorderSize   = 1.0f;
        style.FrameBorderSize   = 1.0f;
        style.TabBorderSize     = 0.0f;

        style.WindowPadding     = ImVec2(10.0f, 10.0f);
        style.FramePadding      = ImVec2(8.0f, 5.0f);
        style.ItemSpacing       = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing  = ImVec2(6.0f, 4.0f);
        style.CellPadding       = ImVec2(6.0f, 4.0f);
        style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
        style.IndentSpacing     = 18.0f;
        style.ScrollbarSize     = 12.0f;
        style.GrabMinSize       = 10.0f;

        // Palette
        const ImVec4 bgDeep          = ImVec4(0.10f, 0.10f, 0.12f, 1.00f); // #19191f
        const ImVec4 bgPanel         = ImVec4(0.14f, 0.14f, 0.17f, 1.00f); // #24242b
        const ImVec4 bgPanelHover    = ImVec4(0.18f, 0.18f, 0.22f, 1.00f); // #2e2e38
        const ImVec4 bgInput         = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
        const ImVec4 bgInputHover    = ImVec4(0.15f, 0.15f, 0.19f, 1.00f);
        const ImVec4 bgInputActive   = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);

        const ImVec4 borderSubtle    = ImVec4(0.23f, 0.23f, 0.28f, 0.85f);
        const ImVec4 borderFocus     = ImVec4(0.35f, 0.48f, 0.95f, 0.90f);

        const ImVec4 accentPrimary   = ImVec4(0.35f, 0.48f, 0.95f, 1.00f); // Indigo #597af2
        const ImVec4 accentHover     = ImVec4(0.45f, 0.58f, 1.00f, 1.00f);
        const ImVec4 accentActive    = ImVec4(0.28f, 0.38f, 0.85f, 1.00f);

        const ImVec4 textPrimary     = ImVec4(0.92f, 0.93f, 0.96f, 1.00f);
        const ImVec4 textSecondary   = ImVec4(0.55f, 0.58f, 0.65f, 1.00f);
        const ImVec4 textDisabled    = ImVec4(0.38f, 0.40f, 0.46f, 1.00f);

        const ImVec4 headerNormal    = ImVec4(0.18f, 0.19f, 0.24f, 1.00f);
        const ImVec4 headerHover     = ImVec4(0.25f, 0.26f, 0.33f, 1.00f);
        const ImVec4 headerActive    = ImVec4(0.30f, 0.32f, 0.40f, 1.00f);

        const ImVec4 tabNormal       = ImVec4(0.13f, 0.13f, 0.16f, 1.00f);
        const ImVec4 tabHovered      = ImVec4(0.22f, 0.23f, 0.29f, 1.00f);
        const ImVec4 tabSelected     = ImVec4(0.18f, 0.19f, 0.25f, 1.00f);
        const ImVec4 tabDimmed       = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
        const ImVec4 tabDimmedSel    = ImVec4(0.15f, 0.15f, 0.19f, 1.00f);

        // Apply Colors
        colors[ImGuiCol_Text]                       = textPrimary;
        colors[ImGuiCol_TextDisabled]               = textDisabled;
        colors[ImGuiCol_WindowBg]                   = bgPanel;
        colors[ImGuiCol_ChildBg]                    = bgDeep;
        colors[ImGuiCol_PopupBg]                    = bgPanel;
        colors[ImGuiCol_Border]                     = borderSubtle;
        colors[ImGuiCol_BorderShadow]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                    = bgInput;
        colors[ImGuiCol_FrameBgHovered]             = bgInputHover;
        colors[ImGuiCol_FrameBgActive]              = bgInputActive;
        colors[ImGuiCol_TitleBg]                    = bgDeep;
        colors[ImGuiCol_TitleBgActive]              = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]           = bgDeep;
        colors[ImGuiCol_MenuBarBg]                  = bgDeep;
        colors[ImGuiCol_ScrollbarBg]                = bgDeep;
        colors[ImGuiCol_ScrollbarGrab]              = ImVec4(0.24f, 0.24f, 0.30f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]       = ImVec4(0.32f, 0.33f, 0.42f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]        = ImVec4(0.40f, 0.42f, 0.52f, 1.00f);
        colors[ImGuiCol_CheckMark]                  = accentPrimary;
        colors[ImGuiCol_SliderGrab]                 = accentPrimary;
        colors[ImGuiCol_SliderGrabActive]           = accentActive;
        colors[ImGuiCol_Button]                     = ImVec4(0.20f, 0.21f, 0.27f, 1.00f);
        colors[ImGuiCol_ButtonHovered]              = ImVec4(0.27f, 0.29f, 0.37f, 1.00f);
        colors[ImGuiCol_ButtonActive]               = accentPrimary;
        colors[ImGuiCol_Header]                     = headerNormal;
        colors[ImGuiCol_HeaderHovered]              = headerHover;
        colors[ImGuiCol_HeaderActive]               = headerActive;
        colors[ImGuiCol_Separator]                  = borderSubtle;
        colors[ImGuiCol_SeparatorHovered]           = accentHover;
        colors[ImGuiCol_SeparatorActive]            = accentActive;
        colors[ImGuiCol_ResizeGrip]                 = ImVec4(0.20f, 0.21f, 0.27f, 0.50f);
        colors[ImGuiCol_ResizeGripHovered]          = accentHover;
        colors[ImGuiCol_ResizeGripActive]           = accentActive;
        colors[ImGuiCol_Tab]                        = tabNormal;
        colors[ImGuiCol_TabHovered]                 = tabHovered;
        colors[ImGuiCol_TabSelected]                = tabSelected;
        colors[ImGuiCol_TabSelectedOverline]        = accentPrimary;
        colors[ImGuiCol_TabDimmed]                  = tabDimmed;
        colors[ImGuiCol_TabDimmedSelected]          = tabDimmedSel;
        colors[ImGuiCol_TabDimmedSelectedOverline]  = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.50f);
        colors[ImGuiCol_DockingPreview]             = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.40f);
        colors[ImGuiCol_DockingEmptyBg]             = bgDeep;
        colors[ImGuiCol_PlotLines]                  = accentPrimary;
        colors[ImGuiCol_PlotLinesHovered]           = accentHover;
        colors[ImGuiCol_PlotHistogram]              = accentPrimary;
        colors[ImGuiCol_PlotHistogramHovered]       = accentHover;
        colors[ImGuiCol_TableHeaderBg]              = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]          = borderSubtle;
        colors[ImGuiCol_TableBorderLight]           = ImVec4(borderSubtle.x, borderSubtle.y, borderSubtle.z, 0.45f);
        colors[ImGuiCol_TableRowBg]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]              = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
        colors[ImGuiCol_TextSelectedBg]             = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.40f);
        colors[ImGuiCol_DragDropTarget]             = ImVec4(1.00f, 0.82f, 0.24f, 0.95f);
        colors[ImGuiCol_NavHighlight]               = accentPrimary;
        colors[ImGuiCol_NavWindowingHighlight]      = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
        colors[ImGuiCol_ModalWindowDimBg]           = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
    }

    void LoadEditorFonts(const std::filesystem::path &resourcesPath) {
        ImGuiIO &io = ImGui::GetIO();
        io.FontGlobalScale = 1.0f; // Reset to 1.0 for sharp, anti-aliased text!

        ImFontConfig fontConfig;
        fontConfig.OversampleH = 2;
        fontConfig.OversampleV = 2;
        fontConfig.PixelSnapH = true;

        std::vector<std::filesystem::path> fontCandidates = {
            resourcesPath / "fonts" / "Roboto-Medium.ttf",
            "run/resources/fonts/Roboto-Medium.ttf",
            "resources/fonts/Roboto-Medium.ttf",
            "../run/resources/fonts/Roboto-Medium.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/usr/share/fonts/Adwaita/AdwaitaSans-Regular.ttf"
        };

        bool loaded = false;
        for (const auto &p : fontCandidates) {
            std::error_code ec;
            if (std::filesystem::exists(p, ec)) {
                ImFont *font = io.Fonts->AddFontFromFileTTF(p.string().c_str(), 16.0f, &fontConfig);
                if (font) {
                    loaded = true;
                    break;
                }
            }
        }

        if (!loaded) {
            io.Fonts->AddFontDefault(&fontConfig);
        }
    }

    ImVec4 GetAssetTypeColor(AssetType type) {
        switch (type) {
            case AssetType::MODEL:    return ThemeColors::AssetModel;
            case AssetType::MESH:     return ThemeColors::AssetMesh;
            case AssetType::MATERIAL: return ThemeColors::AssetMaterial;
            case AssetType::TEXTURE:  return ThemeColors::AssetTexture;
            case AssetType::SHADER:   return ThemeColors::AssetShader;
            case AssetType::REGULAR:  return ThemeColors::AssetRegular;
            default:                  return ThemeColors::AssetRegular;
        }
    }

    const char *GetAssetTypeBadge(AssetType type) {
        switch (type) {
            case AssetType::MODEL:    return "3D";
            case AssetType::MESH:     return "MSH";
            case AssetType::MATERIAL: return "MAT";
            case AssetType::TEXTURE:  return "TEX";
            case AssetType::SHADER:   return "SHD";
            case AssetType::REGULAR:  return "DOC";
            default:                  return "FILE";
        }
    }

    void DrawBadge(const char *text, const ImVec4 &color, float minWidth, const char *tooltip) {
        ImVec2 textSize = ImGui::CalcTextSize(text);
        float lineHeight = ImGui::GetFrameHeight();
        float badgeHeight = textSize.y + 3.0f; // Compact height (~19-20px)
        float paddingX = 6.0f;
        float width = std::max(minWidth, textSize.x + paddingX * 2.0f);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        // Vertically center the compact badge relative to the current frame line
        float offsetY = (lineHeight - badgeHeight) * 0.5f;
        pos.y += offsetY;

        ImDrawList *drawList = ImGui::GetWindowDrawList();

        ImU32 colBg = ImGui::ColorConvertFloat4ToU32(color);
        drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + badgeHeight), colBg, 4.0f);

        // Perfectly centered horizontally and vertically
        float textX = pos.x + (width - textSize.x) * 0.5f;
        float textY = pos.y + (badgeHeight - textSize.y) * 0.5f;
        drawList->AddText(ImVec2(textX, textY), IM_COL32(255, 255, 255, 255), text);

        ImGui::Dummy(ImVec2(width, lineHeight));

        if (tooltip && ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tooltip);
        }
    }

    void DrawAssetBadge(AssetType type, float minWidth) {
        DrawBadge(GetAssetTypeBadge(type), GetAssetTypeColor(type), minWidth);
    }

    bool DrawVec3Control(const char *label, glm::vec3 &values, float resetValue, float columnWidth, float speed) {
        bool changed = false;
        ImGui::PushID(label);

        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 0.0f));

        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight, lineHeight };

        // X Button & Drag
        ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::AxisX);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ThemeColors::AxisXHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ThemeColors::AxisX);
        if (ImGui::Button("X", buttonSize)) {
            values.x = resetValue;
            changed = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        if (ImGui::DragFloat("##X", &values.x, speed, 0.0f, 0.0f, "%.2f")) {
            changed = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Y Button & Drag
        ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::AxisY);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ThemeColors::AxisYHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ThemeColors::AxisY);
        if (ImGui::Button("Y", buttonSize)) {
            values.y = resetValue;
            changed = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        if (ImGui::DragFloat("##Y", &values.y, speed, 0.0f, 0.0f, "%.2f")) {
            changed = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Z Button & Drag
        ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::AxisZ);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ThemeColors::AxisZHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ThemeColors::AxisZ);
        if (ImGui::Button("Z", buttonSize)) {
            values.z = resetValue;
            changed = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        if (ImGui::DragFloat("##Z", &values.z, speed, 0.0f, 0.0f, "%.2f")) {
            changed = true;
        }
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);
        ImGui::PopID();

        return changed;
    }

    void DrawSectionHeader(const char *title) {
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ThemeColors::AccentHover);
        ImGui::TextUnformatted(title);
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::Spacing();
    }
}
