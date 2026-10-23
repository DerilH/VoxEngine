#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/math/UUID.h>
#include <VoxEngine/render/RenderMesh.h>
#include <VoxEngine/resources/MaterialResource.h>
#include <VoxEngine/resources/MeshResource.h>
#include <VoxEngine/scene/Types.h>
#include "RenderLayer.h"
#include "RendererComponent.h"

namespace Vox::Render {
    struct RenderContext;
}

SCENE_NS
    class MeshRendererComponent : public RendererComponent {
        std::function<ConstRef<Resources::MeshResource>()> mMeshProvider;
        ConstRef<Resources::MaterialResource> mMaterial = nullptr;
        RenderLayer mLayer = RenderLayer::WORLD;
        UUID mMeshUuid{};
        UUID mMaterialUuid{};

    public:
        explicit MeshRendererComponent(GameObjectRef gameObject, RenderLayer layer = RenderLayer::WORLD);

        void setMeshProvider(const std::function<ConstRef<Resources::MeshResource>()> &meshProvider, UUID uuid = {}) {
            mMeshProvider = meshProvider;
            mMeshUuid = uuid;
        }

        void setMaterial(ConstRef<Resources::MaterialResource> material, UUID uuid = {}) {
            mMaterial = material;
            mMaterialUuid = uuid;
        }

        UUID getMeshUuid() const noexcept {
            return mMeshUuid;
        }

        UUID getMaterialUuid() const noexcept {
            return mMaterialUuid;
        }

        void setLayer(const RenderLayer layer) {
            mLayer = layer;
        }

        RenderLayer getLayer() const {
            return mLayer;
        }

        void OnAddedToScene(Ref<Scene> scene) override;

        ConstRef<Resources::MeshResource> getMesh() const {
            return mMeshProvider ? mMeshProvider() : nullptr;
        }

        ConstRef<Resources::MaterialResource> getMaterial() const {
            return mMaterial;
        }

        ComponentType type() override {
            return StaticType();
        }

        static ComponentType StaticType() {
            return ComponentType::MESH_RENDERER;
        }
    };

NS_END
