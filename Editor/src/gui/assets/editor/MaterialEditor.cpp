//
// Created by deril on 10/14/26.
//

#include "MaterialEditor.h"

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>
#include "../../../assets/AssetConverter.h"
#include "../../../assets/AssetManager.h"
#include "../../../assets/variants/MaterialAsset.h"
#include "../../Gui.h"
#include "../../Theme.h"

Vox::Editor::MaterialEditor::MaterialEditor(Ref<Engine> engine, UUID assetUuid, ConstRef<Assets::MaterialAsset> asset, bool &shouldClose)
    : AssetEditor(shouldClose), mAssetUuid(assetUuid), mEngine(engine), mOriginalAsset(asset), mAsset(std::move(asset->cloneTyped())) {
    mTextureLink = makeUPtr<AssetLinkComponent>("Texture", AssetType::TEXTURE, [this]() -> InternedString {
        if (!mAsset || !mAsset->albedo.isValid()) return "";
        auto assetMgr = Gui::GetAssetManager();
        if (assetMgr) {
            auto meta = assetMgr->getMetadata(mAsset->albedo);
            if (meta) return meta->fileName;
        }
        return mAsset->albedo.toString();
    });

    mTextureLink->onAssetChanged += [this](UUID texUuid) {
        if (!mAsset) return;
        mAsset->albedo = texUuid;
        auto clone = mAsset->cloneTyped();
        auto assetMgr = Gui::GetAssetManager();
        if (assetMgr) {
            assetMgr->update(mAssetUuid, std::move(mAsset));
            auto matAsset = assetMgr->getOrLoad<Assets::MaterialAsset>(mAssetUuid);
            if (matAsset) {
                Assets::AssetConverter::toResource(*matAsset, mEngine->getResourceManager(), assetMgr, mAssetUuid);
            }
        }
        mAsset = std::move(clone);
    };

    mTextureLink->onChanged += [this](InternedString nameOrPath) {
        if (!mAsset) return;
        auto assetMgr = Gui::GetAssetManager();
        if (assetMgr) {
            UUID texUuid = assetMgr->getUUID(nameOrPath);
            // if (!texUuid.isValid()) texUuid = UUID::fromString(nameOrPath);
            if (texUuid.isValid()) {
                mAsset->albedo = texUuid;
                auto clone = mAsset->cloneTyped();
                assetMgr->update(mAssetUuid, std::move(mAsset));
                auto matAsset = assetMgr->getOrLoad<Assets::MaterialAsset>(mAssetUuid);
                if (matAsset) {
                    Assets::AssetConverter::toResource(*matAsset, mEngine->getResourceManager(), assetMgr, mAssetUuid);
                }
                mAsset = std::move(clone);
            }
        }
    };
}

void Vox::Editor::MaterialEditor::renderInternal(Render::RenderContext context) {
    if (!mAsset) return;

    auto assetMgr = Gui::GetAssetManager();
    auto meta = assetMgr ? assetMgr->getMetadata(mAssetUuid) : nullptr;
    std::string assetName = meta ? meta->fileName.c_str() : "Material";
    std::string assetPath = assetMgr ? assetMgr->getPath(mAssetUuid).c_str() : "";

    bool isOpen = true;
    std::string winTitle = "Material Editor - " + assetName + "###MaterialEditor";
    ImGui::SetNextWindowSize(ImVec2(360, 480), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin(winTitle.c_str(), &isOpen)) {
        ImGui::End();
        if (!isOpen) {
            mShouldClose = true;
        }
        return;
    }

    // Header badge
    ImGui::Spacing();
    DrawAssetBadge(AssetType::MATERIAL);
    ImGui::SameLine();
    ImGui::TextUnformatted(assetName.c_str());
    if (!assetPath.empty()) {
        ImGui::TextDisabled("Path: %s", assetPath.c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    bool changed = false;

    DrawSectionHeader("Textures");
    if (mTextureLink) {
        mTextureLink->render(context);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

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
        auto clone = mAsset->cloneTyped();
        if (assetMgr) {
            assetMgr->update(mAssetUuid, std::move(mAsset));
            auto matAsset = assetMgr->getOrLoad<Assets::MaterialAsset>(mAssetUuid);
            if (matAsset) {
                Assets::AssetConverter::toResource(*matAsset, mEngine->getResourceManager(), assetMgr, mAssetUuid);
            }
        }
        mAsset = std::move(clone);
    }

    ImGui::End();

    if (!isOpen) {
        mShouldClose = true;
    }
}
