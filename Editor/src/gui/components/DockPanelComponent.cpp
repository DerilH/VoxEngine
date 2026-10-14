#include "DockPanelComponent.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace Vox::Editor {
    DockPanelComponent::DockPanelComponent(HashMap<InternedString, std::function<Ref<GuiComponent>()> > options, const InternedString &defaultKey) : mOptions(std::move(options)) {
        if (const auto &res = mOptions[defaultKey]) {
            mSelectedComponent = res();
        }
        mSelectedKey = defaultKey;

        mPanelClass.ClassId = ImHashStr("VoxDockPanel");
        mPanelClass.DockingAllowUnclassed = true;
        mPanelClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

        mChildClass.ClassId = ImHashStr("VoxDockPanelChildren");
        mChildClass.DockingAllowUnclassed = false;
    }

     void DockPanelComponent::renderInternal(Render::RenderContext context) {
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowClass(&mPanelClass);

        constexpr ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        const bool open = ImGui::Begin("##MainDockPanel", nullptr, flags);
        ImGui::PopStyleVar();

        if (open) {
            ImGui::SetCursorPos(ImVec2(4, 4));
            ImGui::Button("##SelectorBtn", ImVec2(18, 18));

            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 4.0f)) {
                ImGuiWindow *window = ImGui::GetCurrentWindow();
                ImGui::ClearActiveID();
                ImGui::StartMouseMovingWindowOrNode(window, window->DockNode, false);
            } else if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered()
                       && ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x == 0.0f) {
                ImGui::OpenPopup("ContentSelectorPopup");
            }

            if (ImGui::BeginPopup("ContentSelectorPopup")) {
                for (auto &key: mOptions | std::views::keys) {
                    const bool isSelected = (mSelectedKey == key);
                    if (ImGui::Selectable(key.c_str(), isSelected)) {
                        if (auto &res = mOptions[key]) {
                            mSelectedKey = key;
                            mSelectedComponent = res();
                        }
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndPopup();
            }

            mInnerDockspaceId = ImGui::GetID("##PanelInnerDockspace");
            ImGui::SetCursorPos(ImVec2(0, 26));
            ImGui::DockSpace(mInnerDockspaceId, ImVec2(0, 0),
                             ImGuiDockNodeFlags_None, &mChildClass);

            if (mSelectedComponent) {
                ImGui::SetNextWindowClass(&mChildClass);
                ImGui::SetNextWindowDockID(mInnerDockspaceId, ImGuiCond_FirstUseEver);
                mSelectedComponent->render(context);
            }
        }
        ImGui::End();
    }
}
