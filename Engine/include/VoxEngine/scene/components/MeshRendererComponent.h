#pragma once
#include <VoxCore/Define.h>
#include <VoxEngine/render/RenderMesh.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>
#include <VoxEngine/resources/assets/MeshAsset.h>
#include <VoxEngine/resources/assets/ShaderAsset.h>
#include <VoxEngine/scene/Types.h>
#include "RendererComponent.h"

namespace Vox::Render {
    struct RenderContext;
}

SCENE_NS
    class MeshRendererComponent : public RendererComponent {
        std::function<Ref<Resources::MeshAsset>()> mMeshProvider;
        ConstRef<Resources::MaterialAsset> mMaterial;
        ConstRef<Resources::ShaderAsset> mVertexShader;
        ConstRef<Resources::ShaderAsset> mFragmentShader;

    public:
        explicit MeshRendererComponent(GameObjectRef gameObject);

        void setMeshProvider(const std::function<Ref<Resources::MeshAsset>()> &meshProvider) {
            mMeshProvider = meshProvider;
        }

        void setMaterial(ConstRef<Resources::MaterialAsset> material) {
            mMaterial = material;
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
