#pragma once
#include <VoxCore/containers/Containers.h>
#include <VoxEngine/Engine.h>
#include <../assets/AssetDirectory.h>

#include "AssetIcon.h"
#include "assets/editor/AssetEditor.h"
#include "components/GuiComponent.h"

namespace Vox::Editor {
    class AssetExplorer : public GuiComponent {
        ConstRef<Resources::AssetDirectory> mCurrentDir = nullptr;
        UPtr<AssetEditor> mAssetEditor = nullptr;
        bool mShouldCloseEditor = false;
        Vector<UPtr<AssetIcon>> mIcons;
        Ref<Engine> mEngine;
        Ref<Resources::AssetManager> mResManager;

        void renderTree();
        void renderDirNode(ConstRef<Resources::AssetDirectory> dir);
        void renderContent();
        void createContentIcons();
    public:
        AssetExplorer(Ref<Engine> engine);

        void openAsset(InternedString path);
        void openAsset(ConstRef<Resources::Asset> asset);

    protected:
        void renderInternal(Render::RenderContext context) override;
    };
}
