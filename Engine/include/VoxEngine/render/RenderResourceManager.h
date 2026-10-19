#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>
#include <VoxEngine/resources/assets/MeshAsset.h>

#include "MaterialPipelines.h"
#include "RenderBackend.h"
#include "RenderMesh.h"
#include "buffers/UniformBufferAllocator.h"

namespace Vox::Resources {
    class ResourceManager;
}

RENDER_NS
    class RenderResourceManager {
    protected:
        HashMap<PipelineStateDesc, Ref<PipelineState> > mPipelineStateByHash;
        HashMap<InternedString, Ref<RenderMesh> > mLoadedMeshes;
        Ref<RenderBackend> mBackend;
        UniformBufferAllocator mUniformAllocator;

        Ref<BindGroup> mCameraGroup;
        Ref<BindGroup> mModelGroup;
        Ref<BindGroup> mSamplerGroup;
        Ref<UniformBuffer> mCameraBuffer;
        Ref<UniformBuffer> mModelBuffer;
        Ref<Texture> mSamplerTexture;
        Ref<Texture> mDefaultTexture;
        size_t mModelAlignment = sizeof(glm::mat4);

        HashMap<InternedString, Ref<Texture>> mLoadedTextures;

        struct MaterialTextureBinding {
            InternedString texturePath;
            Ref<Texture> texture;
            Ref<BindGroup> bindGroup;
        };
        HashMap<ConstRef<Resources::MaterialAsset>, MaterialTextureBinding> mMaterialTextureBindings;

    private:
        virtual Ref<PipelineState> createPSO(ConstRef<PipelineStateDesc> desc) = 0;

    public:
        virtual ~RenderResourceManager() = default;

        explicit RenderResourceManager(Ref<RenderBackend> backend);

        virtual void prepareCache() = 0;

        // Uniform buffer allocator API
        Ref<UniformBuffer> allocateUniformBuffer(size_t size);
        PooledUniformBuffer allocatePooledUniformBuffer(size_t size);
        void freeUniformBuffer(Ref<UniformBuffer> buffer);
        void clearFreeBuffers();

        UniformBufferAllocator& getUniformAllocator() {
            return mUniformAllocator;
        }

        const UniformBufferAllocator& getUniformAllocator() const {
            return mUniformAllocator;
        }

        // Helper for aligning uniform buffer offsets to GPU minimum alignment
        size_t alignUniformOffset(size_t size) const {
            size_t align = mUniformAllocator.getAlignment();
            if (align == 0) return size;
            return (size + align - 1) & ~(align - 1);
        }

        size_t getMinUniformBufferOffsetAlignment() const {
            return mUniformAllocator.getAlignment();
        }

        // Descriptor set creation on demand based on PSO reflection
        virtual Ref<BindGroup> createBindGroup(Ref<PipelineState> pso, uint32_t setId) = 0;
        virtual Ref<BindGroup> createUniformBindGroup(Ref<PipelineState> pso, uint32_t setId, uint32_t binding, Ref<UniformBuffer> buffer) = 0;
        virtual Ref<BindGroup> createTextureBindGroup(Ref<PipelineState> pso, uint32_t setId, uint32_t binding, Ref<Texture> texture) = 0;
        virtual void updateBindGroup(Ref<BindGroup> bindGroup, uint32_t binding, Ref<UniformBuffer> buffer) = 0;
        virtual void updateBindGroupDynamic(Ref<BindGroup> bindGroup, uint32_t binding, Ref<UniformBuffer> buffer, size_t range) = 0;
        virtual void updateBindGroup(Ref<BindGroup> bindGroup, uint32_t binding, Ref<Texture> texture) = 0;

        Ref<BindGroup> getCameraGroup() const {
            return mCameraGroup;
        }

        Ref<BindGroup> getModelGroup() const {
            return mModelGroup;
        }

        Ref<BindGroup> getSamplerGroup() const {
            return mSamplerGroup;
        }

        Ref<Texture> getDefaultTexture() const {
            return mDefaultTexture;
        }

        virtual void updateSampler(Ref<Texture> texture) = 0;

        Ref<Texture> getTexture(InternedString path, Ref<Resources::ResourceManager> resourceManager);
        Ref<BindGroup> getMaterialTextureGroup(ConstRef<Resources::MaterialAsset> material, Ref<PipelineState> pso, Ref<Resources::ResourceManager> resourceManager);

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
