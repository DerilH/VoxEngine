#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>
#include <VoxEngine/resources/assets/MeshAsset.h>

#include "MaterialPipelines.h"
#include "RenderBackend.h"
#include "RenderMesh.h"

RENDER_NS
    class RenderResourceManager {
    protected:
        HashMap<PipelineStateDesc, Ref<PipelineState> > mPipelineStateByHash;
        HashMap<InternedString, Ref<RenderMesh> > mLoadedMeshes;
        Ref<RenderBackend> mBackend;
        Ref<BindGroup> mCameraGroup;
        Ref<BindGroup> mModelGroup;
        Ref<BindGroup> mSamplerGroup;
        Ref<UniformBuffer> mCameraBuffer;
        Ref<UniformBuffer> mModelBuffer;
        Ref<Texture> mSamplerTexture;
        size_t mModelAlignment = sizeof(glm::mat4);

    private:
        virtual Ref<PipelineState> createPSO(ConstRef<PipelineStateDesc> desc) = 0;

    public:
        virtual ~RenderResourceManager() = default;

        explicit RenderResourceManager(Ref<RenderBackend> backend);

        virtual void prepareCache() = 0;

        Ref<BindGroup> getCameraGroup() const {
            return mCameraGroup;
        }

        Ref<BindGroup> getModelGroup() const {
            return mModelGroup;
        }

        Ref<BindGroup> getSamplerGroup() const {
            return mSamplerGroup;
        }

        virtual void updateSampler(Ref<Texture> texture) = 0;

        Ref<UniformBuffer> getCameraBuffer() const {
            return mCameraBuffer;
        }

        Ref<UniformBuffer> getModelBuffer() const {
            return mModelBuffer;
        }

        size_t getModelMatrixAlignment() const {
            return mModelAlignment;
        }

        Ref<PipelineState> getPipeline(ConstRef<Resources::MaterialAsset> material, ConstRef<RenderingState> renderingState);

        Ref<PipelineState> getPipeline(ConstRef<PipelineStateDesc> desc);

        Ref<RenderMesh> getMesh(ConstRef<Resources::MeshAsset> mesh);
    };

NS_END
