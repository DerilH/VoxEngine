#pragma once
#include <VoxCore/containers/Containers.h>
#include <VoxEngine/resources/AssetDirectory.h>

#include "assets/editor/AssetEditor.h"


namespace Vox::Editor {
    class AssetExplorer {
        ConstRef<Resources::AssetDirectory> mCurrentDir = nullptr;
        UPtr<AssetEditor> mAssetEditor = nullptr;
        bool mShouldCloseEditor = false;
    public:
        void render();
        void renderTree();
        void renderDirNode(ConstRef<Resources::AssetDirectory> dir);
        void renderContent();
    };
}
