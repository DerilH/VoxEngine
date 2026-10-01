#pragma once
#include <VoxCore/Define.h>
#include <VoxEngine/render/RenderMesh.h>
#include <VoxEngine/resources/assets/MeshAsset.h>
#include <VoxEngine/resources/assets/ShaderAsset.h>
#include <VoxEngine/scene/Types.h>
#include "RenderableComponent.h"

namespace Vox::Render {
    struct RenderContext;
}

SCENE_NS
    class RenderableMeshComponent : public RenderableComponent {
        std::function<Ref<Resources::MeshAsset>()> mMeshProvider;
        Ref<Resources::ShaderAsset> mVertexShader;
        Ref<Resources::ShaderAsset> mFragmentShader;
    public:
        explicit RenderableMeshComponent(GameObjectRef gameObject);

        void setMeshProvider(std::function<Ref<Resources::MeshAsset>()> meshProvider);
        void setVertexShader(Ref<Resources::ShaderAsset> vertex);
        void setFragmentShader(Ref<Resources::ShaderAsset> fragment);

        void OnAddedToScene(Ref<Scene> scene) override;

        Ref<Resources::MeshAsset> getMesh();
        Ref<Resources::ShaderAsset> getVertexShader();
        Ref<Resources::ShaderAsset> getFragmentShader();
    };

NS_END
