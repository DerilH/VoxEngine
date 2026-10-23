#pragma once
#include <functional>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/math/UUID.h>
#include "GuiComponent.h"
#include "../../assets/variants/AssetType.h"

namespace Vox::Editor {
    class AssetLinkComponent : public GuiComponent {
        std::function<InternedString()> mValueProvider;
        InternedString mLabel;
        AssetType mAssetType = AssetType::REGULAR;
        std::function<bool(InternedString)> mFilter = [](auto _) { return true; };
        bool mFocused = false;

        void renderInternal(Render::RenderContext context) override;

    public:
        Event<UUID> onChanged;
        Event<UUID> onDoubleClicked;
        Event<UUID> onAssetChanged;
        Event<UUID> onAssetDoubleClicked;

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
