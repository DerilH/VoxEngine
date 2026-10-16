//
// Created by deril on 10/15/26.
//
#include "AssetLinkComponent.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.cpp>
#include <utility>

namespace Vox::Editor {
    void AssetLinkComponent::renderInternal(Render::RenderContext context) {
        ImGui::PushID(mFieldName.c_str());
        const auto value = mValueProvider();
        mBuffer.assign(value);

        if (ImGui::InputText(mFieldName.c_str(), &mBuffer)) {
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                if (mFilter(mBuffer)) {
                    onChanged.invoke(mBuffer);
                } else mBuffer.assign(value);
            }
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                mBuffer.assign(static_cast<const char *>(payload->Data), payload->DataSize);
                if (mFilter(mBuffer)) {
                    onChanged.invoke(mBuffer);
                } else mBuffer.assign(value);
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopID();
    }

    AssetLinkComponent::AssetLinkComponent(InternedString name, std::function<InternedString()> valueProvider) : mValueProvider(std::move(valueProvider)), mFieldName(name) {
        mBuffer.reserve(128);
    }
}
