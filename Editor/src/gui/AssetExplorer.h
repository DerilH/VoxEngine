#pragma once
#include <VoxCore/containers/Containers.h>
#include <VoxEngine/resources/AssetDirectory.h>


namespace Vox::Editor {
    class AssetExplorer {
        ConstRef<Resources::AssetDirectory> mCurrentDir;
    public:
        void render();
        void renderTree();
        void renderDirNode(ConstRef<Resources::AssetDirectory> dir);
        void renderContent();
    };
}
