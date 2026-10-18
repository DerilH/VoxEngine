//
// Created by deril on 10/15/26.
//
#include "AssetLinkComponent.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <misc/cpp/imgui_stdlib.cpp>
#include <utility>
#include "../Theme.h"

namespace Vox::Editor {
    AssetLinkComponent::AssetLinkComponent(InternedString label, AssetType assetType, std::function<InternedString()> valueProvider)
        : mValueProvider(std::move(valueProvider)), mLabel(label), mAssetType(assetType) {
        mBuffer.reserve(128);
    }

    AssetLinkComponent::AssetLinkComponent(InternedString label, std::function<InternedString()> valueProvider)
        : AssetLinkComponent(label, AssetType::REGULAR, std::move(valueProvider)) {
    }

    void AssetLinkComponent::renderInternal(Render::RenderContext context) {
        ImGui::PushID(mLabel.c_str());

        const auto value = mValueProvider ? mValueProvider() : InternedString("");
        if (!mFocused) {
            mBuffer.assign(value);
        }

        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, 80.0f);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", mLabel.c_str());
        ImGui::NextColumn();

        DrawAssetBadge(mAssetType);
        bool badgeHovered = ImGui::IsItemHovered();
        bool badgeDoubleClicked = badgeHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
        if (badgeHovered) {
            ImGui::SetTooltip("Double-click to open in Asset Explorer");
        }
        ImGui::SameLine();

        std::string inputId = "##" + std::string(mLabel.c_str());
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        bool entered = ImGui::InputText(inputId.c_str(), &mBuffer, ImGuiInputTextFlags_EnterReturnsTrue);
        mFocused = ImGui::IsItemActive();
        bool inputHovered = ImGui::IsItemHovered();
        bool inputDoubleClicked = inputHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

        if (badgeDoubleClicked || inputDoubleClicked) {
            if (!mBuffer.empty()) {
                onDoubleClicked.invoke(mBuffer);
            }
        }

        if (entered || ImGui::IsItemDeactivatedAfterEdit()) {
            if (mBuffer != value) {
                if (mFilter(mBuffer)) {
                    onChanged.invoke(mBuffer);
                } else {
                    mBuffer.assign(value);
                }
            }
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                const char *draggedPath = static_cast<const char *>(payload->Data);
                size_t dataSize = static_cast<size_t>(payload->DataSize);
                size_t stringLen = (dataSize > 0 && draggedPath[dataSize - 1] == '\0') ? dataSize - 1 : dataSize;
                std::string dropped(draggedPath, stringLen);
                if (mFilter(dropped)) {
                    mBuffer = dropped;
                    onChanged.invoke(mBuffer);
                } else {
                    mBuffer.assign(value);
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::Columns(1);
        ImGui::PopID();
    }
}
