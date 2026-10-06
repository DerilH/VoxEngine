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
#include <VoxEngine/scene/components/MeshRendererComponent.h>
#include <VoxEngine/scene/components/Transform.h>

namespace Vox::Scene {
    class RendererComponent;
}

RENDER_NS
    Renderer::Renderer(Ref<RenderBackend> backend) : mBackend(backend), mRenderResourceManager(new RenderResourceManager(backend)), backendApi(backend->api) {
    }

    void Renderer::init() {
        setBuffering(3);
        createGraph();
    }

    void Renderer::render(Ref<Scene::RendererComponent> el) {
        switch (el->type) {
            case Scene::RenderableType::MESH:
                draw(static_cast<Ref<Scene::MeshRendererComponent>>(el));
                break;
        }
    }

    void Renderer::render(HashSet<Ref<Scene::RendererComponent> > renderable) {
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
        auto geometry = new GeometryPass(GBUFFER_PASS, {{}}, {{color, DISCARD_W_ATTACHMENT}});
        mGraph->addPass(geometry);

    }

    void Renderer::addRenderTarget(RenderTargetRef viewport) {
        mRenderTargets.emplace_back(viewport);
    }

    void Renderer::drawFrame(RenderTargetRef target) {
        color->setExact(target->getBackBuffer());

        auto cmdBuffer = ((Vulkan::Surface *) target)->getCurrentFrame().getCmdBuffer();
        executeGraph(target, cmdBuffer);
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

    void Renderer::draw(Ref<Scene::MeshRendererComponent> component) {
        auto mesh = mRenderResourceManager->getMesh(component->getMesh());
        glm::mat4 identity(1.0f);

        auto mat = component->getMaterial();
        auto vertPath = mat->shaders.at(ShaderStage::VERTEX);
        auto fragPath = mat->shaders.at(ShaderStage::FRAGMENT);
        auto vert = Resources::ResourcesManager::Get().get<Resources::ShaderAsset>(vertPath);
        auto frag = Resources::ResourcesManager::Get().get<Resources::ShaderAsset>(fragPath);

        auto vertexShader = new Shader(&vert->getCompiled());
        auto fragmentShader = new Shader(&frag->getCompiled());

        ShaderState shaderState = ShaderState::GetBuilder().shaders({vertexShader, fragmentShader}).build();
        Vector<BlendState> blendState = BlendState::GetBuilder().startAttachment().endAttachment().build();
        RasterizerState rasterizerState = RasterizerState::GetBuilder().cullMode(mat->cullMode).polygonMode(mat->polygonMode).build();
        MSAAState msaaState = MSAAState(MSAASamples::COUNT_1);
        RenderingState renderingState = RenderingState({mRenderTargets[0]->getBackBuffer()->getFormat()}, Format::UNDEFINED, Format::UNDEFINED);
        auto blend = ArrayView<BlendState>::Copy(blendState);
        PipelineStateDesc desc(shaderState, blend, rasterizerState, mat->topology, msaaState, renderingState);

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

    RenderBackend *Renderer::getBackend() const {
        return mBackend;
    }

    Ref<RenderResourceManager> Renderer::getRenderResourceManager() const {
        return mRenderResourceManager;
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
