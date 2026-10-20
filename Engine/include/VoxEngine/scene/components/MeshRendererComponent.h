#pragma once
#include <VoxCore/Define.h>
#include <VoxEngine/render/RenderMesh.h>
#include <../../../../../Editor/src/assets/MaterialAsset.h>
#include <../../../../../Editor/src/assets/MeshAsset.h>
#include <../../../../../Editor/src/assets/ShaderAsset.h>
#include <VoxEngine/scene/Types.h>
#include "RenderLayer.h"
#include "RendererComponent.h"

namespace Vox::Render {
    struct RenderContext;
}

SCENE_NS
    class MeshRendererComponent : public RendererComponent {
        std::function<ConstRef<Resources::MeshAsset>()> mMeshProvider;
        ConstRef<Resources::MaterialAsset> mMaterial;
        ConstRef<Resources::ShaderAsset> mVertexShader;
        ConstRef<Resources::ShaderAsset> mFragmentShader;
        RenderLayer mLayer = RenderLayer::WORLD;

    public:
        explicit MeshRendererComponent(GameObjectRef gameObject, RenderLayer layer = RenderLayer::WORLD);

        void setMeshProvider(const std::function<ConstRef<Resources::MeshAsset>()> &meshProvider) {
            mMeshProvider = meshProvider;
        }

        void setMaterial(ConstRef<Resources::MaterialAsset> material) {
            mMaterial = material;
        }

        void setLayer(const RenderLayer layer) {
            mLayer = layer;
        }

        RenderLayer getLayer() const {
            return mLayer;
        }

        void OnAddedToScene(Ref<Scene> scene) override;

        ConstRef<Resources::MeshAsset> getMesh() const {
            return mMeshProvider();
        }

        ConstRef<Resources::MaterialAsset> getMaterial() const {
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
