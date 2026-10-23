//
// Created by deril on 3/1/26.
//
#include "VoxEngine/render/Renderer.h"
#include "VoxEngine/Engine.h"
#include "VoxEngine/render/RenderCore.h"
#include "VoxEngine/render/passes/GeometryPass.h"
#include "VoxEngine/render/passes/PassTransition.h"
#include "VoxEngine/render/RenderTarget.h"
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
        onCollect.invoke(this);

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

        for (auto &list: mLayerDrawListByMaterial | std::views::keys) {
            clearDrawList(list);
        }
    }

    void Renderer::setBuffering(char buffers) {
        mBufferingLevel = buffers;
    }

    void Renderer::createGraph() {
        auto old = mGraph;
        mGraph = new RenderGraph(mBackend, old);
        Vector<AttachmentDesc> viewportTextures;
        for (const auto target: mViewportTargets) {
            const auto text = mGraph->importTexture(target.first, [target]() { return target.second->getBackBuffer(); });
            viewportTextures.emplace_back(text, DISCARD_W_ATTACHMENT, true);

            if (target.second->getDepthBuffer() != nullptr) {
                const auto depthText = mGraph->importTexture(target.first + "_Depth", [target]() { return target.second->getDepthBuffer(); });
                viewportTextures.emplace_back(depthText, DISCARD_W_ATTACHMENT, true);
            }
        }

        for (const auto &target: mPresentTargets) {
            mGraph->importTexture(target.first, [target]() { return target.second->getBackBuffer(); });
        }
        auto writes = ArrayView<AttachmentDesc>::Copy(viewportTextures);
        auto geometry = new GeometryPass(GBUFFER_PASS, {{}}, writes);
        mGraph->addPass(geometry);
    }


    Expected<Ref<PipelineState> > Renderer::getPipeline(ConstRef<Resources::MaterialResource> material, ConstRef<RenderingState> renderingState) {
        VOX_ASSERT_PTR(material, "Material is null");
        VOX_ASSERT_PTR(renderingState, "Rendering state is null");

        const auto pipIt = mPipelinesByMaterial.find(material);
        MaterialPipelines pipelines = pipIt == mPipelinesByMaterial.end() ? MaterialPipelines({}) : pipIt->second;
        auto state = pipelines.find(*renderingState);
        if (!state) {
            auto vertHandle = material->getShader(ShaderStage::VERTEX);
            auto fragHandle = material->getShader(ShaderStage::FRAGMENT);
            VOX_EXPECT(vertHandle.isValid(), "Material is missing vertex shader");
            VOX_EXPECT(fragHandle.isValid(), "Material is missing fragment shader");
            auto vert = mResourceManager->get(vertHandle);
            VOX_EXPECT(vert, vert.error());
            auto frag = mResourceManager->get(fragHandle);
            VOX_EXPECT(frag, frag.error());

            auto vertexShader = new Shader(&(*vert)->getCompiled());
            auto fragmentShader = new Shader(&(*frag)->getCompiled());

            auto shaderState = ShaderState::GetBuilder().shaders({vertexShader, fragmentShader}).build();
            VOX_EXPECT(shaderState, shaderState.error());

            Vector<BlendState> blendState = BlendState::GetBuilder().startAttachment().endAttachment().build();
            RasterizerState rasterizerState = RasterizerState::GetBuilder().cullMode(material->getCullMode()).polygonMode(material->getPolygonMode()).build();
            MSAAState msaaState = MSAAState(MSAASamples::COUNT_1);
            DepthState depthState = DepthState::GetBuilder()
                    .depthTest(material->isDepthTest())
                    .depthWrite(material->isDepthWrite())
                    .compareOp(material->getDepthCompareOp())
                    .build();
            auto blend = ArrayView<BlendState>::Copy(blendState);
            PipelineStateDesc desc(shaderState.value(), blend, rasterizerState, material->getTopology(), msaaState, *renderingState, depthState);
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
        mGraph->setViewportSize(target->getSize());
        mGraph->execute({this, cmdBuffer}, target);
    }

    void Renderer::clearDrawList(ConstRef<Resources::MaterialResource> state) {
        for (auto &val: mLayerDrawListByMaterial[state] | std::views::values) {
            val.clear();
        }
    }

    void Renderer::draw(Ref<Scene::MeshRendererComponent> component) {
        auto mesh = mRenderResourceManager->getMesh(component->getMesh());
        if (!mesh) return;
        auto mat = component->getMaterial();
        if (!mat) return;

        auto t = component->gameObject;
        mLayerDrawListByMaterial[mat][static_cast<uint8_t>(component->getLayer())].emplace_back(mesh, t->transform->computeWorldMatrix(), t->id);
    }

    void Renderer::draw(ConstRef<Resources::MeshResource> mesh, ConstRef<Resources::MaterialResource> material, glm::mat4 modelMat, uint8_t layer, uint64_t id) {
        auto m = mRenderResourceManager->getMesh(mesh);
        mLayerDrawListByMaterial[material][layer].emplace_back(m, modelMat, id);
    }

    void Renderer::updateCamera(Ref<Scene::CameraComponent> camera) {
        //TODO make proper cameras
        glm::vec3 cameraPos = camera->gameObject->transform->getPos();
        glm::vec3 cameraTarget = cameraPos + camera->gameObject->transform->getForward();
        glm::vec3 cameraUp = camera->gameObject->transform->getUp();

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

NS_END
