//
// Created by deril on 2/17/26.
//

#pragma once
#include <VoxEngine/render/Renderer.h>
#include <VoxEngine/resources/assets/ModelAsset.h>

#include "VulkanRenderTarget.h"
#include "VulkanState.h"
#include "RenderMesh.h"
#include "VoxEngine/render/graph/RenderGraph.h"

VULKAN_NS
    class VulkanRenderer : public Renderer {
        VulkanState *mVkState = nullptr;
        RenderGraph* mGraph = nullptr;

        void drawFrame(VulkanRenderTarget &target);

    public:
        static std::vector<RenderMesh*> mMeshes;

        explicit VulkanRenderer(Shader::ShaderRepository &shaderRepository);

        void init() override;

        void renderLoop() override;

        void setBuffering(char buffers) override;

        void registerMesh(Resources::ModelAsset* model);

        void recordCommandBuffer(VulkanRenderTarget &targe);
        void addRenderTarget(RenderTarget* target) override;
        void createGraph();
        void addPass(RenderPass* pass);
        RenderGraph& getGraph() const;
    };

NS_END
