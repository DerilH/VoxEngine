//
// Created by deril on 10/6/26.
//

#pragma once
#include "../../components/GuiComponent.h"

namespace Vox::Editor {
    class AssetEditor : public GuiComponent {
    protected:
        bool& mShouldClose;
    public:
        AssetEditor(bool& shouldClose) : mShouldClose(shouldClose) {}
        virtual ~AssetEditor() = default;

    };
}
