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
#include "../../../../ThirdParty/imgui/imgui.h"
#include <functional>

VULKAN_NS
    VulkanRenderer::VulkanRenderer(Shader::ShaderRepository &shaderRepository) : Renderer(shaderRepository), mVkState(nullptr) {
    }

    void VulkanRenderer::drawFrame(VulkanRenderTarget &target) {
        if (!target.begin()) return;

        recordCommandBuffer(target);
        target.submit();
        target.present();
    }

    void VulkanRenderer::recordCommandBuffer(VulkanRenderTarget &target) {

        CommandBuffer commandBuffer = target.getCurrentFrame().getCmdBuffer();
        VkImageView imageView = target.getImageView();
        VkExtent2D extent = target.getExtent();

        commandBuffer.reset();
        commandBuffer.begin();

        VkRenderingAttachmentInfo colorAttachmentInfo{};
        colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachmentInfo.imageView = imageView;
        colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        VkClearValue clearColor = {{0, 0.0f, 0.0f, 0.0f}};
        colorAttachmentInfo.clearValue = clearColor;

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = target.getExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachmentInfo;

        VkImageMemoryBarrier2 imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.srcAccessMask = 0;

        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

        imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        imageBarrier.image = target.getImage();
        imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.levelCount = 1;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = 1;

        VkDependencyInfo depInfo{};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers = &imageBarrier;

        vkCmdPipelineBarrier2(commandBuffer, &depInfo);

        vkCmdBeginRendering(commandBuffer, &renderingInfo);
        mVkState->pipeline->bind(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


        for (const auto &mesh: mMeshes) {
            mesh->use(commandBuffer);
            vkCmdDrawIndexed(commandBuffer, mesh->getIndexCount(), 1, 0, 0, 0);
        }
        mGui(target.getCurrentFrame());

        vkCmdEndRendering(commandBuffer);
        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
        imageBarrier.dstAccessMask = 0;

        imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        imageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        vkCmdPipelineBarrier2(commandBuffer, &depInfo);

        commandBuffer.end();
    }

    void VulkanRenderer::registerMesh(Resources::ModelAsset *asset) {
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
            for (const auto &target: mRenderTargets) {
                drawFrame(*static_cast<VulkanRenderTarget *>(target));
            }

        }
        mVkState->device->waitIdle();

    }

    void VulkanRenderer::setBuffering(char buffers) {
    }
    struct ImDrawVert
    {
        ImVec2  pos;
        ImVec2  uv;
        ImU32   col;
    };
    void VulkanRenderer::addRenderTarget(RenderTarget *target) {
        Renderer::addRenderTarget(target);
        const auto vert = Vox::Resources::ResourcesManager::Get().get<Resources::ShaderAsset>("shaders/baseShader.vert");
        const auto frag = Vox::Resources::ResourcesManager::Get().get<Resources::ShaderAsset>("shaders/baseShader.frag");

        PipelineBuilder builder = mVkState->device->builder<GraphicsPipeline>();

        VkVertexInputBindingDescription binding{};
        binding.binding = 0;
        binding.stride = sizeof(ImDrawVert);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputAttributeDescription attrs[3]{};
        attrs[0].location = 0; // pos
        attrs[0].format = VK_FORMAT_R32G32_SFLOAT;
        attrs[0].offset = offsetof(ImDrawVert, pos);

        attrs[1].location = 1; // uv
        attrs[1].format = VK_FORMAT_R32G32_SFLOAT;
        attrs[1].offset = offsetof(ImDrawVert, uv);

        attrs[2].location = 2; // col
        attrs[2].format = VK_FORMAT_R8G8B8A8_UNORM; // или VK_FORMAT_B8G8R8A8_UNORM
        attrs[2].offset = offsetof(ImDrawVert, col);

        builder.vertexInput(Buffer<VkVertexInputAttributeDescription>(attrs, 3), {binding});
        builder.msaa().rasterizer().topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
        builder.layout(Buffer<VkDescriptorSetLayout>::Empty(), Buffer<VkPushConstantRange>::Empty());
        builder.shader(vert->getCompiled(), VK_SHADER_STAGE_VERTEX_BIT, "main");
        builder.shader(frag->getCompiled(), VK_SHADER_STAGE_FRAGMENT_BIT, "main");
        builder.dynamic({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
        builder.rendering({((VulkanRenderTarget*)target)->getFormat()});
        builder.viewport(1, 1);

        BlendState state = BlendStateBuilder(1).writes(true, true, true, true).endAttachment().build();
        builder.blend(state);



        mVkState->pipeline = new GraphicsPipeline(builder.build());
    }


    // void VulkanRenderer::setBuffering(RenderTarget target, char buffers) {
    // const int maxImages = ->getSwapChain().getImageCount();
    // VOX_VERIFY(buffers <= maxImages, "Trying to set bufferization level more than swapchain supports")
    // createFrames(maxImages);
    // }

NS_END
