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

VULKAN_NS
    VulkanRenderer::VulkanRenderer(Shader::ShaderRepository &shaderRepository) : Renderer(shaderRepository), mVkState(nullptr) {
    }

    void VulkanRenderer::drawFrame(VulkanRenderTarget &target) {
        if (!target.begin()) return;

        recordCommandBuffer(target, []() {});

        target.submit();
        target.present();
    }

    void VulkanRenderer::recordCommandBuffer(VulkanRenderTarget &target, std::function<void()> misc) {
        const auto vert = Vox::Resources::ResourcesManager::Get().get<Resources::ShaderAsset>("shaders/baseShader.vert");
        const auto frag = Vox::Resources::ResourcesManager::Get().get<Resources::ShaderAsset>("shaders/baseShader.frag");

        CommandBuffer commandBuffer = target.getCurrentFrame().getCmdBuffer();
        RenderPass pass = target.getRenderPass(FORWARD_PASS);
        VkFramebuffer framebuffer = target.getFramebuffer(FORWARD_PASS);
        VkExtent2D extent = target.getExtent();

        if(mVkState->pipeline == nullptr) {
            mVkState->pipeline = mVkState->device->createHeap<GraphicsPipeline>(pass, vert->getCompiled(), frag->getCompiled());
        }


        commandBuffer.reset();
        commandBuffer.begin();

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pass.getHandle();
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {1, 0};
        renderPassInfo.renderArea.extent = extent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        mVkState->pipeline->bind(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer.getHandle(), 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;
        vkCmdSetScissor(commandBuffer.getHandle(), 0, 1, &scissor);

        for(const auto& mesh : mMeshes) {
            mesh->use(commandBuffer);
            vkCmdDrawIndexed(commandBuffer.getHandle(), mesh->getIndexCount(), 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffer);
        commandBuffer.end();
    }

    void VulkanRenderer::registerMesh(Resources::ModelAsset* asset) {
//        const std::vector<Vertex> vertices = {
//                {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//                {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
//                {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
//                {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
//        };
//
//        const std::vector<uint32_t> indices = {
//                0, 1, 2, 2, 3, 0
//        };
//
//        auto v = VulkanState::Get()->createVertexBuffer(vertices);
//        auto i = VulkanState::Get()->createIndexBuffer(indices);
//
//        mMeshes.emplace_back(new RenderMesh{v,i});

        for(int i = 0; i < asset->getNestedCount(); i++) {
            auto mesh = asset->getNested<Resources::MeshAsset>(i);


            std::vector<Vertex> vertex;
            for(auto c : mesh->getVertices()) {
                vertex.emplace_back(c, glm::vec3(1,0,1));
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
            for (const auto &target: mRenderTargets) {
                drawFrame(*static_cast<VulkanRenderTarget *>(target));
            }

        }
        mVkState->device->waitIdle();

    }

    void VulkanRenderer::setBuffering(char buffers) {
    }

    // void VulkanRenderer::setBuffering(RenderTarget target, char buffers) {
    // const int maxImages = ->getSwapChain().getImageCount();
    // VOX_VERIFY(buffers <= maxImages, "Trying to set bufferization level more than swapchain supports")
    // createFrames(maxImages);
    // }

NS_END
