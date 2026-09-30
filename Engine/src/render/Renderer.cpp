//
// Created by deril on 3/1/26.
//
#include "VoxEngine/render/Renderer.h"
#include "VoxEngine/render/windowing/Window.h"
#include "VoxEngine/render/RenderCore.h"
#include "VoxEngine/render/passes/GeometryPass.h"
#include "VoxEngine/render/passes/PassTransition.h"
#include "VoxEngine/render/RenderTarget.h"
#include "VoxEngine/resources/assets/MeshAsset.h"
#include "VoxEngine/render/state/Shader.h"
#include <functional>
#include <VoxEngine/render/vulkan/VulkanFrameSync.h>

RENDER_NS
    void Renderer::init() {
        VOX_ASSERT(RenderBackend::Initialized(), "Render backend not initialized");
        setBuffering(3);
        createGraph();
    }

    void Renderer::renderLoop() {
        Time::Update();
        for (const auto& target: mRenderTargets) {
            if (target->beginFrame() == -1) continue;
            drawFrame(target);
            target->endFrame();
        }
        for (auto& list: mDrawListByStateHash | std::views::values) {
            list.clear();
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
        color = mGraph->createTexture();
        mGraph->addPass(new GeometryPass(RenderPassType::GBUFFER_PASS, {{}}, {{color, PassTransition::NONE_W_ATTACHMENT}}));
    }

    void Renderer::addRenderTarget(RenderTargetRef viewport) {
        mRenderTargets.emplace_back(viewport);
    }

    void Renderer::drawFrame(RenderTargetRef target) {
        color->setExact(target->getBackBuffer());

        auto cmdBuffer = ((Vulkan::Surface*) target)->getCurrentFrame().getCmdBuffer();
        cmdBuffer->reset();
        cmdBuffer->begin();

        executeGraph(target, cmdBuffer);

        cmdBuffer->end();
    }

    void Renderer::executeGraph(RenderTargetRef target, CommandBufferRef cmdBuffer) {

        cmdBuffer->setViewportState(0, 0, target->getSize());
        cmdBuffer->setScissor(0, 0, target->getSize());

        mGraph->execute({this, cmdBuffer}, target);
    }

    void Renderer::clearDrawList(const PipelineStateDesc& state) {
        mDrawListByStateHash.at(state);
    }

    const HashMap<PipelineStateDesc, Vector<DrawItem>>& Renderer::getDrawLists() const {
        return mDrawListByStateHash;
    }

    const Vector<DrawItem>& Renderer::getDrawList(const PipelineStateDesc& state) const {
        return mDrawListByStateHash.at(state);
    }


    void Renderer::draw(Resources::ShaderAsset* vertexShaderAsset, Resources::ShaderAsset* fragmentShaderAsset, Resources::MeshAsset* asset) {
        auto vertexShader = new Shader(&vertexShaderAsset->getCompiled());
        auto fragmentShader = new Shader(&fragmentShaderAsset->getCompiled());

        ShaderState shaderState = ShaderState::GetBuilder().shaders({vertexShader, fragmentShader}).build();
        Vector<BlendState> blendState = BlendState::GetBuilder().startAttachment().endAttachment().build();
        RasterizerState rasterizerState = RasterizerState::GetBuilder().build();
        PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;
        MSAAState msaaState;
        RenderingState renderingState = RenderingState({mRenderTargets[0]->getBackBuffer()->getFormat()}, Format::UNDEFINED, Format::UNDEFINED);
        auto blend = ArrayView<BlendState>::Copy(blendState);
        PipelineStateDesc desc(shaderState, blend, rasterizerState, topology, msaaState, renderingState);

        auto it = mMeshes.find(asset->getPath());
        RenderMesh* mesh;
        if (it == mMeshes.end()) {
            VertexBufferRef vBuff = RenderBackend::Get()->createVertexBuffer(data(asset->getVertices()), sizeof(glm::vec3) * asset->getVertices().size(), BufferUsage::VERTEX);
            IndexBufferRef iBuff = RenderBackend::Get()->createIndexBuffer(data(asset->getIndices()), sizeof(uint32_t) * asset->getIndices().size(), IndexType::UINT32);
            mesh = mMeshes[asset->getPath()] = new RenderMesh(vBuff, iBuff);
        } else mesh = it->second;
        mDrawListByStateHash[desc].emplace_back(mesh);
    }

    PipelineStateRef Renderer::getPipelineState(PipelineStateDesc desc) {
        auto it = mPipelineStateByHash.find(desc);
        if (it != mPipelineStateByHash.end()) return it->second;;

        PipelineStateRef pso = RenderBackend::Get()->createPSO(desc);
        mPipelineStateByHash[desc] = pso;
        mDrawListByStateHash[desc] = Vector<DrawItem>();
        return pso;
    }



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