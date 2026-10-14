#include <VoxEngine/render/BindGroup.h>
#include <VoxEngine/render/shaders/ShaderAttribute.h>
#include <VoxEngine/render/shaders/ShaderDataType.h>
#include <VoxEngine/render/state/Shader.h>
#include <VoxEngine/render/vulkan/VulkanBindGroup.h>
#include <VoxEngine/render/vulkan/VulkanRenderResourceManager.h>
#include <VoxEngine/render/vulkan/VulkanUtil.h>
#include <VoxEngine/render/vulkan/VulkanDescriptorSet.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>

VULKAN_NS
#define CAMERA_MATRICES_SIZE sizeof(glm::mat4) * 2
#define CAMERA_MATRICES_BINDING 0
#define CAMERA_MATRICES_SET 2

#define MODEL_MATRICES_SIZE sizeof(glm::mat4)
#define MODEL_MATRICES_BINDING 0
#define MODEL_MATRICES_SET 3

    VkShaderModule createShaderModule(const VulkanDevice &device, const Vector<uint32_t> &code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = sizeof(uint32_t) * code.size();
        createInfo.pCode = code.data();

        VkShaderModule shaderModule;
        VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule), "failed to create shader module!");
        return shaderModule;
    }

    VkFormat getFormatForAttrib(const Shaders::ShaderAttribute &attribute) {
        if (attribute.typeId == Shaders::ShaderDataType::FLOAT) {
            switch (attribute.components) {
                case 1:
                    return VK_FORMAT_R32_SFLOAT;
                case 2:
                    return VK_FORMAT_R32G32_SFLOAT;
                case 3:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case 4:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
        }
        VOX_CHECK(false, "Unsupported attribute type!");
    }

    Vector<VkDescriptorSetLayout> VulkanRenderResourceManager::createDescriptorLayouts(const Vector<ShaderRef> &shaders) {
        // HashMap<uint8_t, HashMap<uint8_t, VkDescriptorSetLayoutBinding> > bindingsBySet;

        Vector<VkDescriptorSetLayout> layouts;
        layouts.resize(4);
        VkDescriptorSetLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = 0;
        VK_CHECK(vkCreateDescriptorSetLayout(*mDevice, &info, nullptr, &layouts[0]), "Failed to create descriptor set layout");
        VK_CHECK(vkCreateDescriptorSetLayout(*mDevice, &info, nullptr, &layouts[1]), "Failed to create descriptor set layout");
        layouts[CAMERA_MATRICES_SET] = mCameraDescSet->getLayouts();
        layouts[MODEL_MATRICES_SET] = mModelDescSet->getLayouts();

        for (const auto shader: shaders) {
            auto compiledShader = shader->getCompiledShader();

            for (const auto &uniform: compiledShader->uniforms) {
                switch (uniform.set) {
                    case CAMERA_MATRICES_SET:
                    case MODEL_MATRICES_SET:
                        break;
                    default:
                        VOX_CHECK_FMT(false, "Invalid uniform set({}) or binding({}) in shader {}, check shader layout conventions", uniform.set, uniform.binding, shader->getCompiledShader()->name)
                }
                // if (binding.stageFlags != 0) {
                // binding.stageFlags |= toVk(shader->getStage());
                // } else {
                // binding.binding = uniform.binding;
                // binding.descriptorCount = uniform.isArray() ? uniform.getArraySize() : 1;
                // binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                // binding.stageFlags = toVk(shader->getStage());
                // binding.pImmutableSamplers = nullptr;
                // }
            }
        }
        //
        // Vector<VkDescriptorSetLayout> layouts(bindingsBySet.size());
        // for (int i = 0; i < bindingsBySet.size(); i++) {
        //     auto &bindings = bindingsBySet[i];
        //
        //     VkDescriptorSetLayoutBinding vkBindings[bindings.size()];
        //     const VkDescriptorSetLayoutBinding *vkBindingsPtr = nullptr;
        //     if (!bindings.empty()) {
        //         // if (i == 0) {
        //         // layouts[i] = device.getGlobalDescriptorSet()->getLayouts();
        //         // continue;
        //         // }
        //
        //         for (const auto [index, binding]: bindings | std::views::values | std::views::enumerate) {
        //             vkBindings[index] = binding;
        //         }
        //         vkBindingsPtr = vkBindings;
        //     }
        //
        //     VkDescriptorSetLayoutCreateInfo layoutInfo{};
        //     layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        //     layoutInfo.bindingCount = bindings.size();
        //     layoutInfo.pBindings = vkBindingsPtr;
        //
        //     VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layouts[i]),
        //              "Failed to create descriptor set layout");
        // }

        return layouts;
    }

    Ref<PipelineState> VulkanRenderResourceManager::createPSO(PipelineStateDesc &desc) {
        VOX_ASSERT(mCachePrepared, "Render resources cache not prepared")
        VkGraphicsPipelineCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        //BLEND STATE
        auto &blend = desc.getBlend();
        VkPipelineColorBlendStateCreateInfo blendState = {};
        createInfo.pColorBlendState = &blendState;

        blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        VkPipelineColorBlendAttachmentState byAttachment[blend.size()];
        for (int i = 0; i < blend.size(); i++) {
            auto state = blend[i];
            auto &attachment = byAttachment[i];
            attachment.blendEnable = state.isBlendEnable();
            attachment.srcColorBlendFactor = toVk(state.getSrcFactor());
            attachment.dstColorBlendFactor = toVk(state.getDstFactor());
            attachment.colorBlendOp = toVk(state.getColorBlendOp());
            attachment.srcAlphaBlendFactor = toVk(state.getSrcAlphaFactor());
            attachment.dstAlphaBlendFactor = toVk(state.getDstAlphaFactor());
            attachment.alphaBlendOp = toVk(state.getAlphaBlendOp());
            attachment.colorWriteMask = state.getColorWriteMask();
        }
        blendState.attachmentCount = blend.size();
        blendState.pAttachments = byAttachment;


        //MSAA
        auto &msaa = desc.getMSAA();
        VkPipelineMultisampleStateCreateInfo msaaState = {};
        createInfo.pMultisampleState = &msaaState;

        msaaState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        msaaState.rasterizationSamples = toVk(msaa.getSamples());
        msaaState.sampleShadingEnable = msaa.isSampleShading();

        //Layout
        auto descriptorLayouts = createDescriptorLayouts(desc.getShader().shaders());
        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        layoutInfo.setLayoutCount = descriptorLayouts.size();
        layoutInfo.pSetLayouts = data(descriptorLayouts);
        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges = nullptr;
        vkCreatePipelineLayout(*mDevice, &layoutInfo, nullptr, &createInfo.layout);

        //Rasterizer
        auto &rasterizer = desc.getRasterizer();
        VkPipelineRasterizationStateCreateInfo rasterizerState = {};
        createInfo.pRasterizationState = &rasterizerState;

        rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerState.depthClampEnable = rasterizer.getDepthClampEnable();
        rasterizerState.rasterizerDiscardEnable = rasterizer.getDiscardEnabled();
        rasterizerState.polygonMode = toVk(rasterizer.getPolygonMode());
        rasterizerState.lineWidth = rasterizer.getLineWidth();
        rasterizerState.cullMode = toVk(rasterizer.getCullMode());
        rasterizerState.frontFace = toVk(rasterizer.getFrontFace());
        rasterizerState.depthBiasEnable = rasterizer.getDepthBiasEnabled();
        rasterizerState.depthBiasConstantFactor = rasterizer.getDepthBiasConstantFactor();
        rasterizerState.depthBiasClamp = rasterizer.getDepthBiasClamp();
        rasterizerState.depthBiasSlopeFactor = rasterizer.getDepthBiasSlopeFactor();

        //Dynamic Rendering
        auto &rendering = desc.getRenderingState();
        VkPipelineRenderingCreateInfo renderingInfo = {};
        createInfo.pNext = &renderingInfo;

        renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

        auto formats = rendering.getColorFormats();
        VkFormat vkFormats[formats.size()];
        renderingInfo.colorAttachmentCount = formats.size();
        renderingInfo.pColorAttachmentFormats = vkFormats;

        for (int i = 0; i < formats.size(); i++) {
            vkFormats[i] = toVk(formats[i]);
        }

        //Shader
        auto &shaderState = desc.getShader();
        auto &shaders = shaderState.shaders();

        VkPipelineShaderStageCreateInfo stages[shaders.size()];
        createInfo.stageCount = shaders.size();
        createInfo.pStages = stages;

        for (int i = 0; i < shaders.size(); i++) {
            auto &stage = stages[i];
            auto shader = shaders[i];

            stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage = toVk(shader->getStage());;
            std::string entry = shader->getEntryPoint();
            stage.pName = "main";
            stage.flags = 0;
            stage.module = createShaderModule(*mDevice, shader->getCompiledShader()->bin);
            stage.pSpecializationInfo = nullptr;
            stage.pNext = nullptr;
        }

        auto vertexShader = std::ranges::find_if(shaders, [&](const auto item) {
            return item->getStage() == ShaderStage::VERTEX;
        });
        VOX_CHECK(vertexShader != shaders.end(), "Vertex shader not provided!");

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        createInfo.pVertexInputState = &vertexInputInfo;

        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto &inputs = (*vertexShader)->getCompiledShader()->inputs;

        VkVertexInputAttributeDescription attributes[inputs.size()];

        int offset = 0;
        for (int i = 0; i < inputs.size(); i++) {
            auto &input = inputs[i];
            attributes[i].location = input.location;
            attributes[i].binding = 0;
            attributes[i].format = getFormatForAttrib(input);
            attributes[i].offset = offset;
            offset += input.components * Shaders::getShaderDataTypeSize(input.typeId);
        }

        VkVertexInputBindingDescription binding[1];
        binding[0].binding = 0;
        binding[0].stride = offset;
        binding[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = binding;
        //
        vertexInputInfo.vertexAttributeDescriptionCount = inputs.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributes;

        //Assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        createInfo.pInputAssemblyState = &inputAssembly;

        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = toVk(desc.getTopology());
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        //RenderPass
        createInfo.renderPass = VK_NULL_HANDLE;
        createInfo.subpass = 0;

        //Dynamic state
        VkPipelineDynamicStateCreateInfo dynamicState{};
        createInfo.pDynamicState = &dynamicState;
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

        //Viewport
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = nullptr;
        viewportState.scissorCount = 1;
        viewportState.pScissors = nullptr;

        createInfo.pViewportState = &viewportState;
        return mDevice->createHeap<VulkanPipelineState>(desc, createInfo);
    }

    VulkanRenderResourceManager::VulkanRenderResourceManager(Ref<RenderBackend> backend, Ref<VulkanDevice> device) : RenderResourceManager(backend), mDevice(device) {
    }

    void VulkanRenderResourceManager::prepareCache() {
        mGlobalDescPool = mDevice->createHeap<VulkanDescriptorPool>(100, ArrayView<VkDescriptorPoolSize>({{toVk(ShaderResourceType::UNIFORM_BUFFER), 100}})); {
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = 1;
            VkDescriptorSetLayoutBinding binding{};
            binding.descriptorCount = 1;
            binding.descriptorType = toVk(ShaderResourceType::UNIFORM_BUFFER);
            binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            createInfo.pBindings = &binding;

            VkDescriptorSetLayout layout;
            vkCreateDescriptorSetLayout(*mDevice, &createInfo, nullptr, &layout);
            mCameraDescSet = mDevice->createHeap<VulkanDescriptorSet>(mGlobalDescPool, layout);
            mCameraBuffer = mDevice->createHeap<VulkanUniformBuffer>(CAMERA_MATRICES_SIZE);
            mCameraDescSet->update(*mDevice, CAMERA_MATRICES_BINDING, *ResourceCast(mCameraBuffer));
            mCameraGroup = new VulkanBindGroup(mCameraDescSet, CAMERA_MATRICES_SET);
        } {
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = 1;
            VkDescriptorSetLayoutBinding binding{};
            binding.descriptorCount = 1;
            binding.descriptorType = toVk(ShaderResourceType::UNIFORM_BUFFER);
            binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            createInfo.pBindings = &binding;

            VkDescriptorSetLayout layout;
            vkCreateDescriptorSetLayout(*mDevice, &createInfo, nullptr, &layout);
            mModelDescSet = mDevice->createHeap<VulkanDescriptorSet>(mGlobalDescPool, layout);
            mModelBuffer = mDevice->createHeap<VulkanUniformBuffer>(MODEL_MATRICES_SIZE);
            mModelDescSet->update(*mDevice, MODEL_MATRICES_BINDING, *ResourceCast(mModelBuffer));
            mModelGroup = new VulkanBindGroup(mModelDescSet, MODEL_MATRICES_SET);
        }
        mCachePrepared = true;
    }

NS_END
