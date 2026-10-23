//
// Created by deril on 10/6/26.
//

#pragma once
#include "../assets/variants/Asset.h"
#include "../assets/variants/import/AssetMetadata.h"

namespace Vox::Editor {
    class AssetIcon {
    public:
        Assets::AssetMetadata mAsset;
        std::function<void()> mDoubleClickCallback = [](){};
        Vector<UPtr<AssetIcon>> mNestedIcons;
    public:
        mutable bool expanded = false;

        explicit AssetIcon(Assets::AssetMetadata mAsset);

        void render() const;
        void setOnDoubleClick(std::function<void()> callback) {
            mDoubleClickCallback = callback;
        }
        const Vector<UPtr<AssetIcon>>& getNested() const {
            return mNestedIcons;
        }
    };
}
