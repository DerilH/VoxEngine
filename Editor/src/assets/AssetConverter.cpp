//
// Created by deril on 2/20/26.
//

#include "AssetConverter.h"
#include <cstring>

ASSETS_NS
Expected<Resources::MeshResource> AssetConverter::convertMesh(ConstRef<MeshAsset> asset) {
    VOX_EXPECT(asset != nullptr, "Null MeshAsset pointer");
    std::vector<glm::vec3> vertices = asset->getVertices();
    Vector<char8_t> interleaved = asset->getInterleaved();
    std::vector<glm::vec3> normals = asset->getNormals();
    std::vector<uint32_t> indices = asset->getIndices();

    Resources::MeshResource res(
        std::move(interleaved),
        std::move(vertices),
        std::move(normals),
        std::move(indices)
    );

    auto valRes = res.validate();
    VOX_EXPECT(valRes, valRes.error());
    return res;
}

Expected<Resources::ShaderResource> AssetConverter::convertShader(ConstRef<ShaderAsset> asset) {
    VOX_EXPECT(asset != nullptr, "Null ShaderAsset pointer");
    Resources::ShaderResource res(asset->getCompiled());
    auto valRes = res.validate();
    VOX_EXPECT(valRes, valRes.error());
    return res;
}

Expected<Resources::TextureResource> AssetConverter::convertTexture(ConstRef<TextureAsset> asset) {
    VOX_EXPECT(asset != nullptr, "Null TextureAsset pointer");
    auto pixelsView = asset->getPixels();
    VOX_EXPECT(!pixelsView.isNull(), "Texture pixels are null");

    Vector<uint8_t> pixels(pixelsView.size());
    std::memcpy(pixels.data(), pixelsView.pData, pixelsView.size());

    Resources::TextureResource res(
        asset->getExtent(),
        Render::Format::RGBA8,
        std::move(pixels)
    );

    auto valRes = res.validate();
    VOX_EXPECT(valRes, valRes.error());
    return res;
}

Expected<Resources::MaterialResource> AssetConverter::convertMaterial(
    ConstRef<MaterialAsset> asset,
    Ref<Resources::ResourceManager> resManager,
    ConstRef<AssetManager> assetManager
) {
    VOX_EXPECT(asset != nullptr, "Null MaterialAsset pointer");
    VOX_EXPECT(resManager != nullptr, "Null ResourceManager pointer");
    VOX_EXPECT(assetManager != nullptr, "Null AssetManager pointer");

    HashMap<Render::ShaderStage, Resources::ShaderHandle> shaderHandles;
    for (const auto &[stage, shaderUuid]: asset->shaders) {
        if (!shaderUuid.isValid()) continue;
        auto shaderAsset = assetManager->getOrLoad<ShaderAsset>(shaderUuid);
        VOX_EXPECT(shaderAsset, "Failed to resolve shader at '{}' for material", shaderUuid.toString().c_str());

        auto handleRes = toResource(*shaderAsset, resManager, shaderUuid);
        VOX_EXPECT(handleRes, handleRes.error());
        shaderHandles[stage] = *handleRes;
    }

    Resources::TextureHandle texHandle{};
    if (asset->albedo.isValid()) {
        auto texAsset = assetManager->getOrLoad<TextureAsset>(asset->albedo);
        if (texAsset) {
            auto handleRes = toResource(*texAsset, resManager, asset->albedo);
            if (handleRes) {
                texHandle = *handleRes;
            }
        }
    }

    Resources::MaterialResource res(
        std::move(shaderHandles),
        asset->polygonMode,
        asset->cullMode,
        asset->topology,
        asset->depthTest,
        asset->depthWrite,
        asset->depthCompareOp,
        texHandle
    );

    auto valRes = res.validate();
    VOX_EXPECT(valRes, valRes.error());
    return res;
}

Expected<Resources::ModelResource> AssetConverter::convertModel(
    ConstRef<ModelAsset> asset,
    Ref<Resources::ResourceManager> resManager
) {
    VOX_EXPECT(asset != nullptr, "Null ModelAsset pointer");
    VOX_EXPECT(resManager != nullptr, "Null ResourceManager pointer");

    Vector<Resources::MeshHandle> meshHandles;
    Vector<InternedString> meshNames;

    for (uint32_t i = 0; i < asset->getNestedCount(); ++i) {
        auto meshAsset = asset->getNestedUnsafe<MeshAsset>(i);
        VOX_EXPECT(meshAsset != nullptr, "Null nested mesh at index {} in model", i);

        auto handleRes = toResource(meshAsset, resManager);
        VOX_EXPECT(handleRes, handleRes.error());

        meshHandles.push_back(*handleRes);
    }

    Resources::ModelResource res("", std::move(meshHandles), std::move(meshNames));
    auto valRes = res.validate();
    VOX_EXPECT(valRes, valRes.error());
    return res;
}

Expected<Resources::MeshHandle> AssetConverter::toResource(
    ConstRef<MeshAsset> asset,
    Ref<Resources::ResourceManager> resManager,
    UUID assetUuid
) {
    VOX_EXPECT(asset != nullptr, "Null MeshAsset pointer");
    VOX_EXPECT(resManager != nullptr, "Null ResourceManager pointer");

    if (assetUuid.isValid()) {
        auto existing = resManager->findByAsset<Resources::MeshResource>(assetUuid);
        if (existing) return *existing;
    }

    auto meshRes = convertMesh(asset);
    VOX_EXPECT(meshRes, meshRes.error());

    if (assetUuid.isValid()) {
        return resManager->addResourceWithAsset<Resources::MeshResource>(assetUuid, std::move(*meshRes));
    }
    return resManager->addResource<Resources::MeshResource>(std::move(*meshRes));
}

Expected<Resources::ShaderHandle> AssetConverter::toResource(
    ConstRef<ShaderAsset> asset,
    Ref<Resources::ResourceManager> resManager,
    UUID assetUuid
) {
    VOX_EXPECT(asset != nullptr, "Null ShaderAsset pointer");
    VOX_EXPECT(resManager != nullptr, "Null ResourceManager pointer");

    if (assetUuid.isValid()) {
        auto existing = resManager->findByAsset<Resources::ShaderResource>(assetUuid);
        if (existing) return *existing;
    }

    auto shaderRes = convertShader(asset);
    VOX_EXPECT(shaderRes, shaderRes.error());

    if (assetUuid.isValid()) {
        return resManager->addResourceWithAsset<Resources::ShaderResource>(assetUuid, std::move(*shaderRes));
    }
    return resManager->addResource<Resources::ShaderResource>(std::move(*shaderRes));
}

Expected<Resources::TextureHandle> AssetConverter::toResource(
    ConstRef<TextureAsset> asset,
    Ref<Resources::ResourceManager> resManager,
    UUID assetUuid
) {
    VOX_EXPECT(asset != nullptr, "Null TextureAsset pointer");
    VOX_EXPECT(resManager != nullptr, "Null ResourceManager pointer");

    if (assetUuid.isValid()) {
        auto existing = resManager->findByAsset<Resources::TextureResource>(assetUuid);
        if (existing) return *existing;
    }

    auto texRes = convertTexture(asset);
    VOX_EXPECT(texRes, texRes.error());

    if (assetUuid.isValid()) {
        return resManager->addResourceWithAsset<Resources::TextureResource>(assetUuid, std::move(*texRes));
    }
    return resManager->addResource<Resources::TextureResource>(std::move(*texRes));
}

Expected<Resources::MaterialHandle> AssetConverter::toResource(
    ConstRef<MaterialAsset> asset,
    Ref<Resources::ResourceManager> resManager,
    ConstRef<AssetManager> assetManager,
    UUID assetUuid
) {
    VOX_EXPECT(asset != nullptr, "Null MaterialAsset pointer");
    VOX_EXPECT(resManager != nullptr, "Null ResourceManager pointer");

    if (assetUuid.isValid()) {
        auto existing = resManager->findByAsset<Resources::MaterialResource>(assetUuid);
        if (existing) return *existing;
    }

    auto matRes = convertMaterial(asset, resManager, assetManager);
    VOX_EXPECT(matRes, matRes.error());

    if (assetUuid.isValid()) {
        return resManager->addResourceWithAsset<Resources::MaterialResource>(assetUuid, std::move(*matRes));
    }
    return resManager->addResource<Resources::MaterialResource>(std::move(*matRes));
}

Expected<Resources::ModelHandle> AssetConverter::toResource(
    ConstRef<ModelAsset> asset,
    Ref<Resources::ResourceManager> resManager,
    UUID assetUuid
) {
    VOX_EXPECT(asset != nullptr, "Null ModelAsset pointer");
    VOX_EXPECT(resManager != nullptr, "Null ResourceManager pointer");

    if (assetUuid.isValid()) {
        auto existing = resManager->findByAsset<Resources::ModelResource>(assetUuid);
        if (existing) return *existing;
    }

    auto modelRes = convertModel(asset, resManager);
    VOX_EXPECT(modelRes, modelRes.error());

    if (assetUuid.isValid()) {
        return resManager->addResourceWithAsset<Resources::ModelResource>(assetUuid, std::move(*modelRes));
    }
    return resManager->addResource<Resources::ModelResource>(std::move(*modelRes));
}

Expected<Resources::ResourceHandleRaw> AssetConverter::getOrConvert(
    UUID assetUuid,
    Ref<Resources::ResourceManager> resManager,
    ConstRef<AssetManager> assetManager
) {
    VOX_EXPECT(assetUuid.isValid(), "Invalid UUID provided");
    VOX_EXPECT(resManager != nullptr, "Null ResourceManager pointer");
    VOX_EXPECT(assetManager != nullptr, "Null AssetManager pointer");

    auto existing = resManager->findByAssetRaw(assetUuid);
    if (existing) {
        return *existing;
    }

    auto meta = assetManager->getMetadata(assetUuid);
    VOX_EXPECT(meta != nullptr, "No metadata found for UUID: {}", assetUuid.toString().c_str());

    switch (meta->type) {
        case AssetType::MESH: {
            auto asset = assetManager->getOrLoad<MeshAsset>(assetUuid);
            VOX_EXPECT(asset, asset.error());
            auto handle = toResource(*asset, resManager, assetUuid);
            VOX_EXPECT(handle, handle.error());
            return static_cast<Resources::ResourceHandleRaw>(*handle);
        }
        case AssetType::MODEL: {
            auto asset = assetManager->getOrLoad<ModelAsset>(assetUuid);
            VOX_EXPECT(asset, asset.error());
            auto handle = toResource(*asset, resManager, assetUuid);
            VOX_EXPECT(handle, handle.error());
            return static_cast<Resources::ResourceHandleRaw>(*handle);
        }
        case AssetType::MATERIAL: {
            auto asset = assetManager->getOrLoad<MaterialAsset>(assetUuid);
            VOX_EXPECT(asset, asset.error());
            auto handle = toResource(*asset, resManager, assetManager, assetUuid);
            VOX_EXPECT(handle, handle.error());
            return static_cast<Resources::ResourceHandleRaw>(*handle);
        }
        case AssetType::SHADER: {
            auto asset = assetManager->getOrLoad<ShaderAsset>(assetUuid);
            VOX_EXPECT(asset, asset.error());
            auto handle = toResource(*asset, resManager, assetUuid);
            VOX_EXPECT(handle, handle.error());
            return static_cast<Resources::ResourceHandleRaw>(*handle);
        }
        case AssetType::TEXTURE: {
            auto asset = assetManager->getOrLoad<TextureAsset>(assetUuid);
            VOX_EXPECT(asset, asset.error());
            auto handle = toResource(*asset, resManager, assetUuid);
            VOX_EXPECT(handle, handle.error());
            return static_cast<Resources::ResourceHandleRaw>(*handle);
        }
        default:
            VOX_EXPECT(false, "Asset type {} cannot be converted to an engine resource", +meta->type);
    }
}

void AssetConverter::convertAll(
    ConstRef<AssetManager> assetManager,
    Ref<Resources::ResourceManager> resManager
) {
    if (!assetManager || !resManager) return;

    for (const auto &[uuid, asset]: assetManager->getLoadedAssets()) {
        if (asset) {
            auto converted = getOrConvert(uuid, resManager, assetManager);
            if (!converted) {
                LOG_WARN("Converting asset failed: {}, {}", converted.error().format(), assetManager->getPath(uuid))
            }
        }
    }
}

NS_END
