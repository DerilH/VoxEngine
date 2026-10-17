//
// Created by deril on 10/14/26.
//

#include "MaterialEditor.h"

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>
#include "../../Theme.h"

namespace Vox::Editor {
    class MaterialAsset;
}

Vox::Editor::MaterialEditor::MaterialEditor(Ref<Engine> engine, ConstRef<Resources::MaterialAsset> asset, bool &shouldClose)
    : AssetEditor(shouldClose), mEngine(engine), mOriginalAsset(asset), mAsset(std::move(asset->cloneTyped())) {
}

template<typename Enum>
requires std::is_enum_v<Enum>
bool RenderEnumCombo(const char *label, Enum &currentValue) {
    constexpr auto values = magic_enum::enum_values<Enum>();
    bool changed = false;

    std::string_view currentName = magic_enum::enum_name(currentValue);

    if (ImGui::BeginCombo(label, currentName.data())) {
        for (const auto &value: values) {
            const bool isSelected = (currentValue == value);
            std::string_view name = magic_enum::enum_name(value);

            if (ImGui::Selectable(name.data(), isSelected)) {
                currentValue = value;
                changed = true;
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}

void Vox::Editor::MaterialEditor::renderInternal(Render::RenderContext context) {
    if (!mAsset) return;

    bool isOpen = true;
    std::string winTitle = "Material Editor - " + std::string(mAsset->getName().c_str()) + "###MaterialEditor";
    ImGui::SetNextWindowSize(ImVec2(360, 420), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin(winTitle.c_str(), &isOpen)) {
        ImGui::End();
        if (!isOpen) {
            mShouldClose = true;
        }
        return;
    }

    // Header badge
    ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::AssetMaterial);
    ImGui::Button("MAT", ImVec2(36, 0));
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::TextUnformatted(mAsset->getName().c_str());
    ImGui::TextDisabled("Path: %s", mAsset->getPath().c_str());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    bool changed = false;

    DrawSectionHeader("Rasterizer State");

    auto polyMode = mAsset->polygonMode;
    if (RenderEnumCombo("Polygon Mode", polyMode)) {
        mAsset->polygonMode = polyMode;
        changed = true;
    }

    auto cullMode = mAsset->cullMode;
    if (RenderEnumCombo("Cull Mode", cullMode)) {
        mAsset->cullMode = cullMode;
        changed = true;
    }

    auto topology = mAsset->topology;
    if (RenderEnumCombo("Topology", topology)) {
        mAsset->topology = topology;
        changed = true;
    }

    DrawSectionHeader("Depth Testing");

    bool depthTest = mAsset->depthTest;
    if (ImGui::Checkbox("Depth Test", &depthTest)) {
        mAsset->depthTest = depthTest;
        changed = true;
    }

    bool depthWrite = mAsset->depthWrite;
    if (ImGui::Checkbox("Depth Write", &depthWrite)) {
        mAsset->depthWrite = depthWrite;
        changed = true;
    }

    auto depthCompareOp = mAsset->depthCompareOp;
    if (RenderEnumCombo("Depth Compare Op", depthCompareOp)) {
        mAsset->depthCompareOp = depthCompareOp;
        changed = true;
    }

    if (changed) {
        auto path = mAsset->getPath();
        auto clone = mAsset->cloneTyped();
        mEngine->getResourceManager()->update(path, std::move(mAsset));
        mAsset = std::move(clone);
    }

    ImGui::End();

    if (!isOpen) {
        mShouldClose = true;
    }
}
