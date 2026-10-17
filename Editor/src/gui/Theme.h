#pragma once

#include <filesystem>
#include <string>
#include <glm/vec3.hpp>
#include <imgui.h>
#include <VoxEngine/resources/assets/AssetType.h>

namespace Vox::Editor {
    namespace ThemeColors {
        // Core accent colors
        constexpr ImVec4 Accent         = ImVec4(0.35f, 0.48f, 0.95f, 1.00f); // #597af2
        constexpr ImVec4 AccentHover    = ImVec4(0.45f, 0.58f, 1.00f, 1.00f);
        constexpr ImVec4 AccentActive   = ImVec4(0.28f, 0.38f, 0.85f, 1.00f);

        // Transform XYZ Colors
        constexpr ImVec4 AxisX          = ImVec4(0.85f, 0.24f, 0.28f, 1.00f);
        constexpr ImVec4 AxisXHover     = ImVec4(0.95f, 0.32f, 0.36f, 1.00f);
        constexpr ImVec4 AxisY          = ImVec4(0.24f, 0.72f, 0.35f, 1.00f);
        constexpr ImVec4 AxisYHover     = ImVec4(0.32f, 0.82f, 0.44f, 1.00f);
        constexpr ImVec4 AxisZ          = ImVec4(0.24f, 0.48f, 0.92f, 1.00f);
        constexpr ImVec4 AxisZHover     = ImVec4(0.32f, 0.58f, 1.00f, 1.00f);

        // Asset Type Badges & Colors
        constexpr ImVec4 AssetModel     = ImVec4(0.22f, 0.68f, 0.92f, 1.00f); // Cyan
        constexpr ImVec4 AssetMesh      = ImVec4(0.30f, 0.60f, 0.88f, 1.00f); // Light blue
        constexpr ImVec4 AssetMaterial  = ImVec4(0.72f, 0.38f, 0.92f, 1.00f); // Violet / Purple
        constexpr ImVec4 AssetTexture   = ImVec4(0.26f, 0.78f, 0.52f, 1.00f); // Emerald
        constexpr ImVec4 AssetShader    = ImVec4(0.96f, 0.58f, 0.22f, 1.00f); // Orange
        constexpr ImVec4 AssetRegular   = ImVec4(0.55f, 0.60f, 0.70f, 1.00f); // Slate
        constexpr ImVec4 AssetDir       = ImVec4(0.95f, 0.75f, 0.25f, 1.00f); // Gold / Amber
    }

    void ApplyVoxTheme();
    void LoadEditorFonts(const std::filesystem::path &resourcesPath);

    ImVec4 GetAssetTypeColor(AssetType type);
    const char *GetAssetTypeBadge(AssetType type);

    bool DrawVec3Control(const char *label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 85.0f, float speed = 0.05f);
    void DrawSectionHeader(const char *title);
}
