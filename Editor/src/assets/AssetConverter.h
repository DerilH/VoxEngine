//
// Created by deril on 2/20/26.
//

#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/Assert.h>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/resources/ResourceHandle.h>

#include "AssetManager.h"
#include "variants/MeshAsset.h"
#include "variants/ModelAsset.h"
#include "variants/ShaderAsset.h"
#include "variants/TextureAsset.h"
#include "variants/MaterialAsset.h"

ASSETS_NS
class AssetConverter {
public:
    static Expected<Resources::MeshResource> convertMesh(ConstRef<MeshAsset> asset);

    static Expected<Resources::ShaderResource> convertShader(ConstRef<ShaderAsset> asset);

    static Expected<Resources::TextureResource> convertTexture(ConstRef<TextureAsset> asset);

    static Expected<Resources::MaterialResource> convertMaterial(
        ConstRef<MaterialAsset> asset,
        Ref<Resources::ResourceManager> resManager,
        ConstRef<AssetManager> assetManager
    );

    static Expected<Resources::ModelResource> convertModel(
        ConstRef<ModelAsset> asset,
        Ref<Resources::ResourceManager> resManager
    );

    static Expected<Resources::MeshHandle> toResource(
        ConstRef<MeshAsset> asset,
        Ref<Resources::ResourceManager> resManager,
        UUID assetUuid = {}
    );

    static Expected<Resources::ShaderHandle> toResource(
        ConstRef<ShaderAsset> asset,
        Ref<Resources::ResourceManager> resManager,
        UUID assetUuid = {}
    );

    static Expected<Resources::TextureHandle> toResource(
        ConstRef<TextureAsset> asset,
        Ref<Resources::ResourceManager> resManager,
        UUID assetUuid = {}
    );

    static Expected<Resources::MaterialHandle> toResource(
        ConstRef<MaterialAsset> asset,
        Ref<Resources::ResourceManager> resManager,
        ConstRef<AssetManager> assetManager,
        UUID assetUuid = {}
    );

    static Expected<Resources::ModelHandle> toResource(
        ConstRef<ModelAsset> asset,
        Ref<Resources::ResourceManager> resManager,
        UUID assetUuid = {}
    );

    // Generic conversion by UUID
    static Expected<Resources::ResourceHandleRaw> getOrConvert(
        UUID assetUuid,
        Ref<Resources::ResourceManager> resManager,
        ConstRef<AssetManager> assetManager
    );

    // Batch conversion of all assets currently loaded in AssetManager
    static void convertAll(
        ConstRef<AssetManager> assetManager,
        Ref<Resources::ResourceManager> resManager
    );
};

NS_END
