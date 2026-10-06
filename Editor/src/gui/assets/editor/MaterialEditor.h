//
// Created by deril on 10/6/26.
//

#pragma once

#include <VoxCore/Pointers.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>

#include "AssetEditor.h"

namespace Vox::Editor {
    class MaterialEditor : public AssetEditor {
        ConstRef<Resources::MaterialAsset> mOriginalAsset;
        UPtr<Resources::MaterialAsset> mAsset;

    public:
        MaterialEditor(ConstRef<Resources::MaterialAsset> asset, bool& shouldClose);

        void render() override;
    };
}
