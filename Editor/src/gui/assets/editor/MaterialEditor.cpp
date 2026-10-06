//
// Created by deril on 10/6/26.
//

#include "MaterialEditor.h"

#include <imgui.h>
#include <VoxEngine/resources/ResourcesManager.h>

namespace Vox::Resources {
    class MaterialAsset;
}

Vox::Editor::MaterialEditor::MaterialEditor(ConstRef<Resources::MaterialAsset> asset, bool& shouldClose) : AssetEditor(shouldClose), mOriginalAsset(asset), mAsset(std::move(asset->cloneTyped())) {
}

template<typename EnumType>
bool RenderEnumCombo(const char *label, EnumType &currentVal) {
    bool changed = false;
    constexpr auto entries = magic_enum::enum_entries<EnumType>();

    std::string_view currentName = magic_enum::enum_name(currentVal);

    if (ImGui::BeginCombo(label, currentName.data())) {
        for (const auto &[value, name]: entries) {
            bool isSelected = (currentVal == value);
            if (ImGui::Selectable(name.data(), isSelected)) {
                currentVal = value;
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


void Vox::Editor::MaterialEditor::render() {
    if (!mAsset) return;
    bool isOpen = true;
    ImGui::Begin("MaterialAsset", &isOpen);
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

    ImGui::Separator();
    ImGui::Text("Shaders");

    for (auto &[stage, shaderPath]: mAsset->shaders) {
        std::string stageName = std::string(magic_enum::enum_name(stage));
        ImGui::PushID(static_cast<int>(stage));

        char pathBuffer[512] = "";
        strncpy(pathBuffer, shaderPath.c_str(), sizeof(pathBuffer));
        if (ImGui::InputText("Material", pathBuffer, IM_ARRAYSIZE(pathBuffer))) {
            shaderPath = InternedString(pathBuffer);
            changed = true;
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                const char *draggedPath = static_cast<const char *>(payload->Data);
                strncpy(pathBuffer, draggedPath, sizeof(pathBuffer) - 1);
                pathBuffer[sizeof(pathBuffer) - 1] = '\0';
                changed = true;
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopID();
    }
    if (changed) {
        auto path = mAsset->getPath();
        auto clone = mAsset->cloneTyped();
        Resources::ResourcesManager::Get().update(path, std::move(mAsset));
        mAsset = std::move(clone);
    }
    ImGui::End();
}
