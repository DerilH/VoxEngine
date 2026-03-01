//
// Created by deril on 2/17/26.
//

#include <VoxEngine/render/vulkan/VulkanRenderer.h>
#include <VoxCore/Time.h>
#include <VoxEngine/render/vulkan/VulkanState.h>
#include "VoxEngine/render/vulkan/Vertex.h"
#include "VoxEngine/resources/assets/ModelAsset.h"
#include "VoxEngine/resources/assets/MeshAsset.h"
#include "VoxCore/SingletonBase.h"
#include "VoxEngine/resources/ResourcesManager.h"
#include "VoxEngine/resources/assets/ShaderAsset.h"
#include "VoxCore/containers/Buffer.h"
#include "VoxEngine/render/vulkan/passes/GeometryPass.h"
#include "VoxEngine/render/vulkan/passes/PresentPass.h"
#include <functional>

VULKAN_NS
    void cmdSetViewport(const CommandBuffer& cmdBuff, float width, float height) {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmdBuff, 0, 1, &viewport);
    }

    void cmdSetScissor(const CommandBuffer& cmdBuff, float width, float height) {
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = VkExtent2D(width, height);
        vkCmdSetScissor(cmdBuff, 0, 1, &scissor);
    }

    std::vector<RenderMesh*> VulkanRenderer::mMeshes;


    VulkanRenderer::VulkanRenderer(Shader::ShaderRepository& shaderRepository) : Renderer(shaderRepository), mVkState(nullptr) {
    }

    void VulkanRenderer::drawFrame(VulkanRenderTarget& target) {
        if (!target.begin()) return;

        recordCommandBuffer(target);
        target.submit();
        target.present();
    }

    void VulkanRenderer::recordCommandBuffer(VulkanRenderTarget& target) {

        CommandBuffer commandBuffer = target.getCurrentFrame().getCmdBuffer();

        commandBuffer.reset();
        commandBuffer.begin();

        cmdSetViewport(commandBuffer, extent.width, extent.height);
        cmdSetScissor(commandBuffer, extent.width, extent.height);

        mGraph->execute(commandBuffer,target);

        commandBuffer.end();

    }

    void VulkanRenderer::registerMesh(Resources::ModelAsset* asset) {
        for (int i = 0; i < asset->getNestedCount(); i++) {
            auto mesh = asset->getNested<Resources::MeshAsset>(i);


            std::vector<Vertex> vertex;
            for (auto c: mesh->getVertices()) {
                vertex.emplace_back(c, glm::vec3(1, 0, 1));
            }

            auto v = VulkanState::Get()->createVertexBuffer(vertex);
            auto indices = VulkanState::Get()->createIndexBuffer(mesh->getIndices());
            mMeshes.emplace_back(new RenderMesh(v, indices));
        }
    }

    void VulkanRenderer::init() {
        VulkanState::initialize(VK_API_VERSION_1_4, mShaderRepository);
        mVkState = VulkanState::Get();

        setBuffering(3);
    }

    void VulkanRenderer::renderLoop() {

        while (!mShouldStop) {
            Time::Update();
            glfwPollEvents();
            for (const auto& target: mRenderTargets) {
                drawFrame(*static_cast<VulkanRenderTarget*>(target));
            }

        }
        mVkState->device->waitIdle();

    }

    void VulkanRenderer::setBuffering(char buffers) {
    }

    void VulkanRenderer::addRenderTarget(RenderTarget* target) {
        Renderer::addRenderTarget(target);
        const auto vert = Vox::Resources::ResourcesManager::Get().get<Resources::ShaderAsset>("shaders/baseShader.vert");
        const auto frag = Vox::Resources::ResourcesManager::Get().get<Resources::ShaderAsset>("shaders/baseShader.frag");

        PipelineBuilder builder = mVkState->device->builder<GraphicsPipeline>();

        builder.vertexInput(Vertex::getAttributeDescriptions(), {Vertex::getBindingDescription()});
        builder.msaa().rasterizer().topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
        builder.layout(Buffer<VkDescriptorSetLayout>::Empty(), Buffer<VkPushConstantRange>::Empty());
        builder.shader(vert->getCompiled(), VK_SHADER_STAGE_VERTEX_BIT, "main");
        builder.shader(frag->getCompiled(), VK_SHADER_STAGE_FRAGMENT_BIT, "main");
        builder.dynamic({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
        builder.rendering({target->getFormat()});
        builder.viewport(1, 1);

        BlendState state = BlendStateBuilder(1).writes(true, true, true, true).endAttachment().build();
        builder.blend(state);

        mVkState->pipeline = new GraphicsPipeline(builder.build());
        if (mGraph == nullptr) {
            createGraph();
        }
    }

    void VulkanRenderer::createGraph() {
        mGraph = new RenderGraph(*VulkanState::Get()->device);
        auto color = mGraph->createTexture();
        mRenderTargets.get
        auto present = mGraph->importTexture(mRenderTargets.at(0));

        mGraph->addPass(new GeometryPass(RenderPassType::GBUFFER_PASS, {}, {{color, PassTransition::NONE_W_ATTACHMENT}}));
        mGraph->addPass(new PresentPass(RenderPassType::PRESENT_PASS, {{color, PassTransition::W_ATTACHMENT_R_COPY}}, {{present, NONE_W_COPY}}));
    }

    RenderGraph& VulkanRenderer::getGraph() const {
        return *mGraph;
    }

    void VulkanRenderer::addPass(RenderPass* pass) {
        mGraph->addPass(pass);
    }


    // void VulkanRenderer::setBuffering(RenderTarget target, char buffers) {
    // const int maxImages = ->getSwapChain().getImageCount();
    // VOX_VERIFY(buffers <= maxImages, "Trying to set bufferization level more than swapchain supports")
    // createFrames(maxImages);
    // }

NS_END
