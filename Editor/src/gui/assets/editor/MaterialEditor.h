//
// Created by deril on 10/6/26.
//

#pragma once

#include <VoxCore/Pointers.h>
#include <VoxEngine/Engine.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>

#include "AssetEditor.h"

namespace Vox::Editor {
    class MaterialEditor : public AssetEditor {
        ConstRef<Resources::MaterialAsset> mOriginalAsset;
        UPtr<Resources::MaterialAsset> mAsset;
        Ref<Engine> mEngine;
    public:
        MaterialEditor(Ref<Engine> engine, ConstRef<Resources::MaterialAsset> asset, bool& shouldClose);

        void renderInternal(Render::RenderContext context) override;
    };
}
