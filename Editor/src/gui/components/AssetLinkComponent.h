#pragma once
#include "GuiComponent.h"

namespace Vox::Editor {
    class AssetLinkComponent : public GuiComponent {
        std::string mBuffer;
        std::function<InternedString()> mValueProvider;
        InternedString mFieldName;
        std::function<bool(InternedString)> mFilter = [](auto _) {return true;};

        void renderInternal(Render::RenderContext context) override;

    public:
        Event<InternedString> onChanged;

        explicit AssetLinkComponent(InternedString name, std::function<InternedString()> valueProvider);

        void setFilter(const std::function<bool(InternedString)> &filter) {
            mFilter = filter;
        }
    };
}
