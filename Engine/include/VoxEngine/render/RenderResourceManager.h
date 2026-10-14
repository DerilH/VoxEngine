#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxEngine/resources/assets/MeshAsset.h>

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
        Ref<UniformBuffer> mCameraBuffer;
        Ref<UniformBuffer> mModelBuffer;

    private:
        virtual Ref<PipelineState> createPSO(PipelineStateDesc &desc) = 0;

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

        Ref<UniformBuffer> getCameraBuffer() const {
            return mCameraBuffer;
        }

        Ref<UniformBuffer> getModelBuffer() const {
            return mModelBuffer;
        }

        Ref<PipelineState> getPipeline(PipelineStateDesc desc);

        Ref<RenderMesh> getMesh(ConstRef<Resources::MeshAsset> mesh);
    };

NS_END
