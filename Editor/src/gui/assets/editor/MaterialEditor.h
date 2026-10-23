//
// Created by deril on 10/6/26.
//

#pragma once

#include <VoxCore/Pointers.h>
#include <VoxCore/math/UUID.h>
#include <VoxEngine/Engine.h>
#include "../../../assets/variants/MaterialAsset.h"

#include "AssetEditor.h"
#include "../../components/AssetLinkComponent.h"

namespace Vox::Editor {
    class MaterialEditor : public AssetEditor {
        UUID mAssetUuid{};
        ConstRef<Assets::MaterialAsset> mOriginalAsset;
        UPtr<Assets::MaterialAsset> mAsset;
        Ref<Engine> mEngine;
        UPtr<AssetLinkComponent> mTextureLink;

    public:
        MaterialEditor(Ref<Engine> engine, UUID assetUuid, ConstRef<Assets::MaterialAsset> asset, bool& shouldClose);

        void renderInternal(Render::RenderContext context) override;
    };
}
