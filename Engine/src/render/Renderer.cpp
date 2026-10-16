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
#include "VoxEngine/render/state/DepthState.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <VoxEngine/render/buffers/UniformBuffer.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>
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

    void Renderer::render(Ref<Scene::Scene> scene) {
        Time::Update();
        for (const auto el: scene->getAllRenderables()) {
            render(el);
        }
        if (const auto activeCamera = scene->getActiveCamera()) {
            updateCamera(activeCamera);
        }

        if (!mPresentTargets.empty()) {
            for (const auto &presenter: mPresentTargets | std::views::values) {
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

    void Renderer::createGraph() {
        auto old = mGraph;
        mGraph = new RenderGraph(old);
        Vector<AttachmentDesc> viewportTextures;
        for (const auto target: mViewportTargets) {
            const auto text = mGraph->importTexture(target.first, [target]() { return target.second->getBackBuffer(); });
            viewportTextures.emplace_back(text, DISCARD_W_ATTACHMENT);

            if (target.second->getDepthBuffer() != nullptr) {
                const auto depthText = mGraph->importTexture(target.first + "_Depth", [target]() { return target.second->getDepthBuffer(); });
                viewportTextures.emplace_back(depthText, DISCARD_W_ATTACHMENT);
            }
        }

        for (const auto& target: mPresentTargets) {
            mGraph->importTexture(target.first, [target]() { return target.second->getBackBuffer(); });
        }
        auto writes = ArrayView<AttachmentDesc>::Copy(viewportTextures);
        auto geometry = new GeometryPass(GBUFFER_PASS, true, {{}}, writes);
        mGraph->addPass(geometry);
    }


    Expected<Ref<PipelineState> > Renderer::getPipeline(ConstRef<Resources::MaterialAsset> material, ConstRef<RenderingState> renderingState) {
        const auto pipIt = mPipelinesByMaterial.find(material);
        MaterialPipelines pipelines = pipIt == mPipelinesByMaterial.end() ? MaterialPipelines({}) : pipIt->second;
        auto state = pipelines.find(*renderingState);
        if (!state) {
            auto vertPath = material->shaders.at(ShaderStage::VERTEX);
            auto fragPath = material->shaders.at(ShaderStage::FRAGMENT);
            auto vert = mResourceManager->get<Resources::ShaderAsset>(vertPath);
            VOX_EXPECT(vert, vert.error());
            auto frag = mResourceManager->get<Resources::ShaderAsset>(fragPath);
            VOX_EXPECT(frag, frag.error());

            auto vertexShader = new Shader(&(*vert)->getCompiled());
            auto fragmentShader = new Shader(&(*frag)->getCompiled());

            auto shaderState = ShaderState::GetBuilder().shaders({vertexShader, fragmentShader}).build();
            VOX_EXPECT(shaderState, shaderState.error());

            Vector<BlendState> blendState = BlendState::GetBuilder().startAttachment().endAttachment().build();
            RasterizerState rasterizerState = RasterizerState::GetBuilder().cullMode(material->cullMode).polygonMode(material->polygonMode).build();
            MSAAState msaaState = MSAAState(MSAASamples::COUNT_1);
            DepthState depthState = DepthState::GetBuilder()
                .depthTest(material->depthTest)
                .depthWrite(material->depthWrite)
                .compareOp(material->depthCompareOp)
                .build();
            auto blend = ArrayView<BlendState>::Copy(blendState);
            PipelineStateDesc desc(shaderState.value(), blend, rasterizerState, material->topology, msaaState, *renderingState, depthState);
            pipelines.insert({*renderingState, desc});
            return mRenderResourceManager->getPipeline(&desc);
        }
        return mRenderResourceManager->getPipeline(state);
    }

    void Renderer::addViewportTarget(InternedString name, RenderTargetRef viewport) {
        mViewportTargets[name] = viewport;
    }

    void Renderer::addPresentTarget(InternedString name, Ref<RenderTarget> target) {
        mPresentTargets.emplace(name, target);
    }

    void Renderer::drawFrame(RenderTargetRef target, Ref<CommandBuffer> cmdBuffer) {
        executeGraph(target, cmdBuffer);
    }

    void Renderer::executeGraph(RenderTargetRef target, CommandBufferRef cmdBuffer) {
        mGraph->execute({this, cmdBuffer}, target);
    }

    void Renderer::clearDrawList(ConstRef<Resources::MaterialAsset> state) {
        mDrawListByStateHash[state].clear();
    }

    const HashMap<ConstRef<Resources::MaterialAsset>, Vector<DrawItem> > &Renderer::getDrawLists() const {
        return mDrawListByStateHash;
    }

    const Vector<DrawItem> &Renderer::getDrawList(const ConstRef<Resources::MaterialAsset> &state) const {
        return mDrawListByStateHash.at(state);
    }

    void Renderer::draw(Ref<Scene::MeshRendererComponent> component) {
        auto mesh = mRenderResourceManager->getMesh(component->getMesh());
        glm::mat4 modelMat(1.0f);

        auto mat = component->getMaterial();

        auto t = component->gameObject;
        modelMat = glm::translate(modelMat, t->transform->getPos());
        modelMat = modelMat * glm::mat4_cast(t->transform->getRotation());
        modelMat = glm::scale(modelMat, t->transform->getScale());

        mDrawListByStateHash[mat].emplace_back(mesh, modelMat);
    }

    void Renderer::updateCamera(Ref<Scene::CameraComponent> camera) {
        //TODO make proper cameras
        glm::vec3 cameraPos = camera->gameObject->transform->getPos();
        glm::vec3 cameraTarget = cameraPos + camera->gameObject->transform->getForward();
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        auto view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        float fov = glm::radians(45.0f);
        float aspect = 1400.0f / 900.0f;
        float zNear = 0.1f;
        float zFar = 100.0f;

        auto proj = glm::perspective(fov, aspect, zNear, zFar);
        // proj[1][1] *= -1.0f;

        auto buff = mRenderResourceManager->getCameraBuffer();
        buff->write(&proj, sizeof(glm::mat4));
        buff->write(&view, sizeof(glm::mat4), sizeof(glm::mat4));
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
