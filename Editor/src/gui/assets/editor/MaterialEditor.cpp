//
// Created by deril on 10/14/26.
//

#include "MaterialEditor.h"

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>

namespace Vox::Editor {
    class MaterialAsset;
}

Vox::Editor::MaterialEditor::MaterialEditor(Ref<Engine> engine, ConstRef<Resources::MaterialAsset> asset, bool &shouldClose) : AssetEditor(shouldClose), mEngine(engine), mOriginalAsset(asset), mAsset(std::move(asset->cloneTyped())) {
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
    if (!isOpen) {
        mShouldClose = true;
    }
    bool changed = false;
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

    ImGui::Separator();
    ImGui::Text("Shaders");


    if (changed) {
        auto path = mAsset->getPath();
        auto clone = mAsset->cloneTyped();
        mEngine->getResourceManager()->update(path, std::move(mAsset));
        mAsset = std::move(clone);
    }
}
