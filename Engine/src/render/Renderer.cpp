//
// Created by deril on 3/1/26.
//
#include "VoxEngine/render/Renderer.h"
#include "VoxEngine/Engine.h"
#include "VoxEngine/render/RenderCore.h"
#include "VoxEngine/render/passes/GeometryPass.h"
#include "VoxEngine/render/passes/PassTransition.h"
#include "VoxEngine/render/RenderTarget.h"
#include "../../Editor/src/gui/Gui.h"
#include "VoxEngine/resources/assets/MeshAsset.h"
#include "VoxEngine/render/state/Shader.h"
#include <VoxEngine/render/vulkan/VulkanFrameSync.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/VulkanDescriptorSet.h>
#include <VoxEngine/render/vulkan/buffers/VulkanUniformBuffer.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/scene/components/RenderableMeshComponent.h>
#include <VoxEngine/scene/components/Transform.h>

namespace Vox::Scene {
    class RenderableComponent;
}

RENDER_NS
    Renderer::Renderer(RenderBackend *backend) : mBackend(backend), backendApi(backend->api) {
    }

    void Renderer::init() {
        setBuffering(3);
        createGraph();
    }

    void Renderer::render(Ref<Scene::RenderableComponent> el) {
        switch (el->type) {
            case Scene::RenderableType::MESH:
                draw(static_cast<Ref<Scene::RenderableMeshComponent>>(el));
                break;
        }
    }

    void Renderer::render(HashSet<Ref<Scene::RenderableComponent> > renderable) {
        Time::Update();
        for (const auto el: renderable) {
            render(el);
        }

        for (const auto &target: mRenderTargets) {
            if (target->beginFrame() == -1) continue;
            drawFrame(target);
            target->endFrame();
        }
        for (auto &list: mDrawListByStateHash | std::views::values) {
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
        color = mGraph->createTexture("Color");
        auto geometry = new GeometryPass(RenderPassType::GBUFFER_PASS, {{}}, {{color, PassTransition::NONE_W_ATTACHMENT}});
        mGraph->addPass(geometry);
    }

    void Renderer::addRenderTarget(RenderTargetRef viewport) {
        mRenderTargets.emplace_back(viewport);
    }

    void Renderer::drawFrame(RenderTargetRef target) {
        color->setExact(target->getBackBuffer());

        auto cmdBuffer = ((Vulkan::Surface *) target)->getCurrentFrame().getCmdBuffer();
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

    void Renderer::clearDrawList(const PipelineStateDesc &state) {
        mDrawListByStateHash.at(state);
    }

    const HashMap<PipelineStateDesc, Vector<DrawItem> > &Renderer::getDrawLists() const {
        return mDrawListByStateHash;
    }

    const Vector<DrawItem> &Renderer::getDrawList(const PipelineStateDesc &state) const {
        return mDrawListByStateHash.at(state);
    }

    void Renderer::draw(Ref<Scene::RenderableMeshComponent> component) {
        auto vertexShader = new Shader(&component->getVertexShader()->getCompiled());
        auto fragmentShader = new Shader(&component->getFragmentShader()->getCompiled());

        ShaderState shaderState = ShaderState::GetBuilder().shaders({vertexShader, fragmentShader}).build();
        Vector<BlendState> blendState = BlendState::GetBuilder().startAttachment().endAttachment().build();
        RasterizerState rasterizerState = RasterizerState::GetBuilder().cullMode(CullMode::NONE).polygonMode(PolygonMode::FILL).build();
        PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;
        MSAAState msaaState;
        RenderingState renderingState = RenderingState({mRenderTargets[0]->getBackBuffer()->getFormat()}, Format::UNDEFINED, Format::UNDEFINED);
        auto blend = ArrayView<BlendState>::Copy(blendState);
        PipelineStateDesc desc(shaderState, blend, rasterizerState, topology, msaaState, renderingState);

        auto asset = component->getMesh();
        auto it = mMeshes.find(component->getMesh()->getPath());
        RenderMesh *mesh;
        if (it == mMeshes.end()) {
            VertexBufferRef vBuff = mBackend->createVertexBuffer(data(asset->getVertices()), sizeof(glm::vec3) * asset->getVertices().size(), BufferUsage::VERTEX);
            IndexBufferRef iBuff = mBackend->createIndexBuffer(data(asset->getIndices()), sizeof(uint32_t) * asset->getIndices().size(), IndexType::UINT32);
            mesh = mMeshes[asset->getPath()] = new RenderMesh(vBuff, iBuff);

            auto ubo = mBackend->createUniformBuffer(sizeof(glm::mat4));

            mesh->setModelUbo(ubo);

            auto vkDevice = Vulkan::ResourceCast(mBackend->getDevice());
            auto layout = vkDevice->getModelDescriptorSetLayout();
            auto set = vkDevice->createDescriptorSet(layout);
            set->update(*vkDevice, 0, *dynamic_cast<Vulkan::VulkanUniformBuffer *>(ubo));
            mesh->setDescriptorSet(set);
            dynamic_cast<Vulkan::VulkanUniformBuffer *>(ubo)->setDescriptorSet(set);
        } else mesh = it->second;

        glm::mat4 identity(1.0f);

        auto t = component->gameObject;
        identity = glm::translate(identity, t->transform->getPos());
        identity = identity * glm::mat4_cast(t->transform->getRotation());
        identity = glm::scale(identity, t->transform->getScale());
        mesh->getModelUbo()->write(&identity, sizeof(identity));

        mDrawListByStateHash[desc].emplace_back(mesh);
    }

    void Renderer::draw(Ref<Resources::ShaderAsset> vertexShaderAsset, Ref<Resources::ShaderAsset> fragmentShaderAsset, Resources::ModelAsset *model) {
        for (int i = 0; i < model->getNestedCount(); i++) {
            auto mesh = model->getNested<Resources::MeshAsset>(i);
            // draw(TODO);
        }
    }

    PipelineStateRef Renderer::getPipelineState(PipelineStateDesc desc) {
        auto it = mPipelineStateByHash.find(desc);
        if (it != mPipelineStateByHash.end()) return it->second;;

        PipelineStateRef pso = mBackend->createPSO(desc);
        mPipelineStateByHash[desc] = pso;
        mDrawListByStateHash[desc] = Vector<DrawItem>();
        return pso;
    }

    RenderBackend *Renderer::getBackend() const {
        return mBackend;
    }

    RenderGraph &Renderer::getGraph() {
        return *mGraph;
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
