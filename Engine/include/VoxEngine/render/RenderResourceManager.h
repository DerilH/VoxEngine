#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxEngine/resources/assets/MeshAsset.h>

#include "RenderBackend.h"
#include "RenderMesh.h"

RENDER_NS
    class RenderResourceManager {
        HashMap<PipelineStateDesc, Ref<PipelineState>> mPipelineStateByHash;
        HashMap<InternedString, Ref<RenderMesh> > mLoadedMeshes;
        Ref<RenderBackend> mBackend;

    public:
        explicit RenderResourceManager(Ref<RenderBackend> backend);

        Ref<PipelineState> getPipeline(PipelineStateDesc desc);

        Ref<RenderMesh> getMesh(ConstRef<Resources::MeshAsset> mesh);
    };

NS_END
