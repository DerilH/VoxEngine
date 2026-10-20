//
// Created by deril on 2/20/26.
//

#pragma once

#include "AssetImporter.h"
#include <VoxEngine/render/shaders/ShaderCompiler.h>

ASSETS_NS
    class ShaderImporter : public AssetImporter {
        static Vox::Render::Shaders::ShaderCompiler compiler;
    public:
        Ref<Asset> load(InternedString path, ArrayView<void> data) override;
        AssetType type() override {
            return AssetType::SHADER;
        }
    };
NS_END
