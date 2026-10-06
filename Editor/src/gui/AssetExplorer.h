#pragma once
#include <VoxCore/containers/Containers.h>
#include <VoxEngine/resources/AssetDirectory.h>

#include "AssetIcon.h"
#include "assets/editor/AssetEditor.h"


namespace Vox::Editor {

    class AssetExplorer {
        ConstRef<Resources::AssetDirectory> mCurrentDir = nullptr;
        UPtr<AssetEditor> mAssetEditor = nullptr;
        bool mShouldCloseEditor = false;
        Vector<UPtr<AssetIcon>> mIcons;

        void renderTree();
        void renderDirNode(ConstRef<Resources::AssetDirectory> dir);
        void renderContent();
        void createContentIcons();
    public:
        void render();

    };
}
