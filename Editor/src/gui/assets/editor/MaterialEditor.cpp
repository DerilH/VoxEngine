//
// Created by deril on 10/6/26.
//

#include "MaterialEditor.h"

#include <imgui.h>
#include <VoxEngine/resources/ResourceManager.h>

#include "../../components/AssetLinkComponent.h"

namespace Vox::Resources {
    class MaterialAsset;
}

Vox::Editor::MaterialEditor::MaterialEditor(Ref<Engine> engine, ConstRef<Resources::MaterialAsset> asset, bool &shouldClose) : AssetEditor(shouldClose), mEngine(engine), mOriginalAsset(asset), mAsset(std::move(asset->cloneTyped())) {
    for (auto &[stage, shaderPath]: mAsset->shaders) {
        std::string stageName = std::string(magic_enum::enum_name(stage));
        auto a = makeUPtr<AssetLinkComponent>(stageName, [stage, this]() { return mAsset->shaders[stage]; });
        a->onChanged += [this, stage](auto path) {
            mAsset->shaders[stage] = path;
            auto p = mAsset->getPath();
            auto clone = mAsset->cloneTyped();
            mEngine->getResourceManager()->update(p, std::move(mAsset));
            mAsset = std::move(clone);
        };
        a->setFilter([this, stage](InternedString a) {
            auto shader = mEngine->getResourceManager()->get<Resources::ShaderAsset>(a);
            if (!shader) return false;
            return shader.value()->getCompiled().stage == stage;
        });
        mChilds.emplace(stageName, std::move(a));
    }
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

    ImGui::Separator();
    ImGui::Text("Shaders");


    if (changed) {
        auto path = mAsset->getPath();
        auto clone = mAsset->cloneTyped();
        mEngine->getResourceManager()->update(path, std::move(mAsset));
        mAsset = std::move(clone);
    }
}
