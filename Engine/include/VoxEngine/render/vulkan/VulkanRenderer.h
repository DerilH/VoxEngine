//
// Created by deril on 2/17/26.
//

#pragma once
#include <VoxEngine/render/Renderer.h>
#include <VoxEngine/resources/assets/ModelAsset.h>

#include "VulkanRenderTarget.h"
#include "VulkanState.h"
#include "RenderMesh.h"

VULKAN_NS
    class VulkanRenderer : public Renderer {
        friend class RendererFactory;
        VulkanState *mVkState;
        std::vector<RenderMesh*> mMeshes;


        void drawFrame(VulkanRenderTarget &target);

    public:
        explicit VulkanRenderer(Shader::ShaderRepository &shaderRepository);

        void init() override;

        void renderLoop() override;

        void setBuffering(char buffers) override;

        void registerMesh(Resources::ModelAsset* model);

        void recordCommandBuffer(VulkanRenderTarget &targe);
        void addRenderTarget(RenderTarget* target) override;
    };

NS_END
