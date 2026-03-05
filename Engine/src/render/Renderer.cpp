//
// Created by deril on 3/1/26.
//
#include "VoxEngine/render/Renderer.h"
#include "VoxEngine/render/windowing/Window.h"
#include "VoxEngine/render/RenderCore.h"
#include "VoxEngine/render/passes/RenderPass.h"
#include "VoxEngine/render/passes/GeometryPass.h"
#include "VoxEngine/render/passes/PassTransition.h"
#include "VoxEngine/render/RenderTarget.h"

RENDER_NS
    void Renderer::init() {
        VOX_ASSERT(RenderBackend::Initialized(), "Render backend not initialized");
        setBuffering(3);
    }

    void Renderer::renderLoop() {
        while (!mShouldStop) {
            Time::Update();
            for (const auto& target: mRenderTargets) {
                target->beginFrame();
                drawFrame(target);
                target->endFrame();
            }
        }
    }

    void Renderer::setBuffering(char buffers) {
        mBufferingLevel = buffers;
    }

    void Renderer::stop() {
        mShouldStop = true;
    }

    void Renderer::createGraph() {
        mGraph = new RenderGraph();
        auto color = mGraph->createTexture();
        mGraph->addPass(new GeometryPass(RenderPassType::GBUFFER_PASS, {{}}, {{color, PassTransition::NONE_W_ATTACHMENT}}));
    }

    void Renderer::addRenderTarget(RenderTargetRef viewport) {
        mRenderTargets.emplace_back(viewport);
    }

    void Renderer::drawFrame(RenderTargetRef target) {
        CommandBufferRef cmdBuffer = nullptr;
        cmdBuffer->reset();
        cmdBuffer->begin();

        cmdBuffer->beginDrawingTarget(target);
        executeGraph(target, cmdBuffer);
        cmdBuffer->endDrawingTarget(target);

        cmdBuffer->end();
    }

    void Renderer::executeGraph(RenderTargetRef target, CommandBufferRef cmdBuffer) {


        cmdBuffer->setViewportState(0,0,target->getSize());
        cmdBuffer->setScissor(0,0,target->getSize());

        mGraph->execute(cmdBuffer,target);
    }


//void VulkanRenderer::registerMesh(Resources::ModelAsset* asset) {
//    for (int i = 0; i < asset->getNestedCount(); i++) {
//        auto mesh = asset->getNested<Resources::MeshAsset>(i);
//
//
//        std::vector<Vertex> vertex;
//        for (auto c: mesh->getVertices()) {
//            vertex.emplace_back(c, glm::vec3(1, 0, 1));
//        }
//
//        auto v = VulkanState::Get()->createVertexBuffer(vertex);
//        auto indices = VulkanState::Get()->createIndexBuffer(mesh->getIndices());
//        mMeshes.emplace_back(new RenderMesh(v, indices));
//    }
//}

//    void VulkanRenderer::addRenderTarget(RenderTarget* target) {
//        Renderer::addRenderTarget(target);
//        const auto vert = Vox::Resources::ResourcesManager::Get().get<Resources::ShaderAsset>("shaders/baseShader.vert");
//        const auto frag = Vox::Resources::ResourcesManager::Get().get<Resources::ShaderAsset>("shaders/baseShader.frag");
//
//        PipelineBuilder builder = mVkState->device->builder<GraphicsPipeline>();
//
//        builder.vertexInput(Vertex::getAttributeDescriptions(), {Vertex::getBindingDescription()});
//        builder.msaa().rasterizer().topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
//        builder.layout(Buffer<VkDescriptorSetLayout>::Empty(), Buffer<VkPushConstantRange>::Empty());
//        builder.shader(vert->getCompiled(), VK_SHADER_STAGE_VERTEX_BIT, "main");
//        builder.shader(frag->getCompiled(), VK_SHADER_STAGE_FRAGMENT_BIT, "main");
//        builder.dynamic({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
//        builder.rendering({target->getFormat()});
//        builder.viewport(1, 1);
//
//        BlendState state = BlendStateBuilder(1).writes(true, true, true, true).endAttachment().build();
//        builder.blend(state);
//
//        mVkState->pipeline = new GraphicsPipeline(builder.build());
//        if (mGraph == nullptr) {
//            createGraph();
//        }
//    }
NS_END