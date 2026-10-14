#pragma once
#include <functional>

#include "GuiComponent.h"
#include <imgui.h>

namespace Vox::Editor {
    class DockPanelComponent : public GuiComponent{
        HashMap<InternedString, std::function<Ref<GuiComponent>()>> mOptions;
        InternedString mSelectedKey;
        Ref<GuiComponent> mSelectedComponent = nullptr;

    public:
        explicit DockPanelComponent(HashMap<InternedString, std::function<Ref<GuiComponent>()>> options, const InternedString& defaultKey = "");
        Ref<GuiComponent> getSelectedComponent() const { return mSelectedComponent; }

    protected:
        void renderInternal(Render::RenderContext context) override;
        ImGuiWindowClass mPanelClass;
        ImGuiWindowClass mChildClass;
        ImGuiID mInnerDockspaceId = 0;

    };
}
