//
// Created by deril on 10/6/26.
//

#pragma once
#include "../assets/variants/Asset.h"

namespace Vox::Editor {
    class AssetIcon {
    public:
        ConstRef<Assets::Asset> mAsset;
        std::function<void()> mDoubleClickCallback = [](){};
        Vector<UPtr<AssetIcon>> mNestedIcons;
    public:
        mutable bool expanded = false;

        explicit AssetIcon(ConstRef<Assets::Asset> asset);

        void render() const;
        void setOnDoubleClick(std::function<void()> callback) {
            mDoubleClickCallback = callback;
        }
        const Vector<UPtr<AssetIcon>>& getNested() const {
            return mNestedIcons;
        }
    };
}
