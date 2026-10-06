//
// Created by deril on 10/6/26.
//

#pragma once

namespace Vox::Editor {
    class AssetEditor {
    protected:
        bool& mShouldClose;
    public:
        AssetEditor(bool& shouldClose) : mShouldClose(shouldClose) {}
        virtual ~AssetEditor() = default;

        virtual void render() = 0;

    };
}