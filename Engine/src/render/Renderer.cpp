//
// Created by deril on 3/1/26.
//
#include "VoxEngine/render/Renderer.h"
#include "VoxEngine/Engine.h"
#include "VoxEngine/render/RenderCore.h"
#include "VoxEngine/render/passes/GeometryPass.h"
#include "VoxEngine/render/passes/PassTransition.h"
#include "VoxEngine/render/RenderTarget.h"
#include "VoxEngine/resources/assets/MeshAsset.h"
#include "VoxEngine/render/state/Shader.h"
#include <VoxEngine/render/vulkan/SwapChainFrameSync.h>
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
    Renderer::Renderer(Ref<Resources::ResourceManager> resManager, Ref<RenderResourceManager> renderResManager, Ref<RenderBackend> backend) : mBackend(backend), mRenderResourceManager(renderResManager), mResourceManager(resManager), backendApi(backend->api) {
    }

    void Renderer::init() {
        setBuffering(3);
        createGraph();
        mRenderResourceManager->prepareCache();
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
        updateCamera();
        if (mPresentTargets.size() > 0) {
            for (const auto &presenter: mPresentTargets) {
                for (const auto &target: mViewportTargets | std::ranges::views::values) {
                    if (presenter->beginFrame() == -1) continue;
                    drawFrame(target, presenter->getCmdBuffer());
                    presenter->endFrame();
                }
            }
        } else {
            VOX_CHECK(false, "Headless render mode is not implemented")
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
        auto old = mGraph;
        mGraph = new RenderGraph(old);
        int i = 0;
        auto viewportTextures = (AttachmentDesc *) malloc(sizeof(AttachmentDesc) * mViewportTargets.size());
        for (const auto target: mViewportTargets) {
            const auto text = mGraph->createTexture(target.first);
            new(&viewportTextures[i]) AttachmentDesc{text, DISCARD_W_ATTACHMENT};
            text->setExact([target]() { return target.second->getBackBuffer(); });
            i++;
        }

        auto geometry = new GeometryPass(GBUFFER_PASS, true, {{}}, ArrayView(viewportTextures, mViewportTargets.size()));
        mGraph->addPass(geometry);
    }

    void Renderer::addViewportTarget(InternedString name, RenderTargetRef viewport) {
        mViewportTargets[name] = viewport;
    }

    void Renderer::addPresentTarget(Ref<RenderTarget> target) {
        mPresentTargets.emplace_back(target);
    }

    void Renderer::drawFrame(RenderTargetRef target, Ref<CommandBuffer> cmdBuffer) {
        executeGraph(target, cmdBuffer);
    }

    void Renderer::executeGraph(RenderTargetRef target, CommandBufferRef cmdBuffer) {
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
        glm::mat4 modelMat(1.0f);

        auto mat = component->getMaterial();
        auto vertPath = mat->shaders.at(ShaderStage::VERTEX);
        auto fragPath = mat->shaders.at(ShaderStage::FRAGMENT);
        auto vert = mResourceManager->get<Resources::ShaderAsset>(vertPath);
        auto frag = mResourceManager->get<Resources::ShaderAsset>(fragPath);

        auto vertexShader = new Shader(&vert->getCompiled());
        auto fragmentShader = new Shader(&frag->getCompiled());

        ShaderState shaderState = ShaderState::GetBuilder().shaders({vertexShader, fragmentShader}).build();
        Vector<BlendState> blendState = BlendState::GetBuilder().startAttachment().endAttachment().build();
        RasterizerState rasterizerState = RasterizerState::GetBuilder().cullMode(mat->cullMode).polygonMode(mat->polygonMode).build();
        MSAAState msaaState = MSAAState(MSAASamples::COUNT_1);
        RenderingState renderingState = RenderingState({mViewportTargets.begin()->second->getBackBuffer()->getFormat()}, Format::UNDEFINED, Format::UNDEFINED);
        auto blend = ArrayView<BlendState>::Copy(blendState);
        PipelineStateDesc desc(shaderState, blend, rasterizerState, mat->topology, msaaState, renderingState);

        auto t = component->gameObject;
        modelMat = glm::translate(modelMat, t->transform->getPos());
        modelMat = modelMat * glm::mat4_cast(t->transform->getRotation());
        modelMat = glm::scale(modelMat, t->transform->getScale());

        mDrawListByStateHash[desc].emplace_back(mesh, modelMat);
    }

    void Renderer::updateCamera() {
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        auto view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        float fov = glm::radians(45.0f);
        float aspect = 1400.0f / 900.0f;
        float zNear = 0.1f;
        float zFar = 100.0f;

        auto proj = glm::perspective(fov, aspect, zNear, zFar);
        proj[1][1] *= -1.0f;

        auto buff = mRenderResourceManager->getCameraBuffer();
        buff->write(&proj, sizeof(glm::mat4));
        buff->write(&view, sizeof(glm::mat4), sizeof(glm::mat4));
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
