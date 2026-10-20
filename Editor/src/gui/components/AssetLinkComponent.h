#pragma once
#include <functional>
#include <VoxCore/containers/Containers.h>
#include <../../assets/Asset.h>
#include "GuiComponent.h"

namespace Vox::Editor {
    class AssetLinkComponent : public GuiComponent {
        std::string mBuffer;
        std::function<InternedString()> mValueProvider;
        InternedString mLabel;
        AssetType mAssetType = AssetType::REGULAR;
        std::function<bool(InternedString)> mFilter = [](auto _) { return true; };
        bool mFocused = false;

        void renderInternal(Render::RenderContext context) override;

    public:
        Event<InternedString> onChanged;
        Event<InternedString> onDoubleClicked;

        explicit AssetLinkComponent(InternedString label, AssetType assetType, std::function<InternedString()> valueProvider);
        explicit AssetLinkComponent(InternedString label, std::function<InternedString()> valueProvider);

        void setFilter(const std::function<bool(InternedString)> &filter) {
            mFilter = filter;
        }

        void setAssetType(AssetType type) {
            mAssetType = type;
        }

        void setValueProvider(std::function<InternedString()> valueProvider) {
            mValueProvider = std::move(valueProvider);
        }
    };
}
