#include <VoxEngine/render/BindGroup.h>
#include <VoxEngine/render/shaders/ShaderAttribute.h>
#include <VoxEngine/render/shaders/ShaderDataType.h>
#include <VoxEngine/render/state/Shader.h>
#include <VoxEngine/render/vulkan/VulkanBindGroup.h>
#include <VoxEngine/render/vulkan/VulkanRenderResourceManager.h>
#include <VoxEngine/render/vulkan/VulkanUtil.h>
#include <VoxEngine/render/vulkan/VulkanDescriptorSet.h>
#include <VoxEngine/render/vulkan/VulkanDescriptorPool.h>
#include <VoxEngine/render/vulkan/state/VulkanPipelineState.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>
#include <map>

VULKAN_NS
#define SAMPLER_BINDING 0
#define SAMPLER_SET 1

#define CAMERA_MATRICES_SIZE (sizeof(glm::mat4) * 2)
#define CAMERA_MATRICES_BINDING 0
#define CAMERA_MATRICES_SET 2

#define MODEL_MATRICES_SIZE sizeof(glm::mat4)
#define MODEL_MATRICES_BINDING 0
#define MODEL_MATRICES_SET 3

    static VkShaderModule createShaderModule(const VulkanDevice &device, const Vector<uint32_t> &code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = sizeof(uint32_t) * code.size();
        createInfo.pCode = code.data();

        VkShaderModule shaderModule;
        VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule), "failed to create shader module!");
        return shaderModule;
    }

    static VkFormat getFormatForAttrib(const Shaders::ShaderAttribute &attribute) {
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

    static uint64_t hashBindings(const Vector<DescriptorBindingInfo>& bindings) {
        uint64_t h = bindings.size();
        for (const auto& b : bindings) {
            uint64_t part = (static_cast<uint64_t>(b.binding) << 48) |
                            (static_cast<uint64_t>(b.resourceType) << 32) |
                            (static_cast<uint64_t>(b.stageFlags) << 16) |
                            static_cast<uint64_t>(b.count);
            h ^= part + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        }
        return h;
    }

    VulkanRenderResourceManager::VulkanRenderResourceManager(Ref<RenderBackend> backend, Ref<VulkanDevice> device)
        : RenderResourceManager(backend), mDevice(device) {
        VkDeviceSize minAlign = mDevice->getPhysicalDevice().getMinUniformBufferOffsetAlignment();
        if (minAlign > 0) {
            mUniformAllocator.setAlignment(minAlign);
        }
    }

    VulkanRenderResourceManager::~VulkanRenderResourceManager() {
        if (mEmptyDescriptorSetLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(*mDevice, mEmptyDescriptorSetLayout, nullptr);
            mEmptyDescriptorSetLayout = VK_NULL_HANDLE;
        }

        for (auto [hash, layout] : mDescriptorSetLayoutCache) {
            if (layout != VK_NULL_HANDLE) {
                vkDestroyDescriptorSetLayout(*mDevice, layout, nullptr);
            }
        }
        mDescriptorSetLayoutCache.clear();
    }

    Ref<VulkanDescriptorPool> VulkanRenderResourceManager::getOrCreateDescriptorPool() {
        if (mDescriptorPools.empty()) {
            auto pool = mDevice->createHeap<VulkanDescriptorPool>(256, ArrayView<VkDescriptorPoolSize>({
                {toVk(ShaderResourceType::COMBINED_IMAGE_SAMPLER), 256},
                {toVk(ShaderResourceType::UNIFORM_BUFFER), 256},
                {toVk(ShaderResourceType::UNIFORM_BUFFER_DYNAMIC), 256},
                {toVk(ShaderResourceType::STORAGE_BUFFER), 128}
            }));
            mDescriptorPools.push_back(pool);
        }
        return mDescriptorPools.back();
    }

    VkDescriptorSetLayout VulkanRenderResourceManager::getOrCreateDescriptorSetLayout(const Vector<DescriptorBindingInfo>& bindings) {
        uint64_t hash = hashBindings(bindings);
        auto it = mDescriptorSetLayoutCache.find(hash);
        if (it != mDescriptorSetLayoutCache.end()) {
            return it->second;
        }

        Vector<VkDescriptorSetLayoutBinding> vkBindings;
        for (const auto& b : bindings) {
            VkDescriptorSetLayoutBinding vkb{};
            vkb.binding = b.binding;
            vkb.descriptorType = toVk(b.resourceType);
            vkb.descriptorCount = b.count > 0 ? b.count : 1;
            vkb.stageFlags = b.stageFlags != 0 ? b.stageFlags : (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
            vkBindings.push_back(vkb);
        }

        VkDescriptorSetLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = static_cast<uint32_t>(vkBindings.size());
        info.pBindings = vkBindings.empty() ? nullptr : vkBindings.data();

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        VK_CHECK(vkCreateDescriptorSetLayout(*mDevice, &info, nullptr, &layout), "Failed to create descriptor set layout");
        mDescriptorSetLayoutCache[hash] = layout;
        return layout;
    }

    Ref<PipelineState> VulkanRenderResourceManager::createPSO(ConstRef<PipelineStateDesc> desc) {
        VOX_ASSERT(mCachePrepared, "Render resources cache not prepared")
        VkGraphicsPipelineCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        // BLEND STATE
        auto &blend = desc->getBlend();
        VkPipelineColorBlendStateCreateInfo blendState = {};
        createInfo.pColorBlendState = &blendState;

        blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        VkPipelineColorBlendAttachmentState byAttachment[blend.empty() ? 1 : blend.size()];
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

        // MSAA
        auto &msaa = desc->getMSAA();
        VkPipelineMultisampleStateCreateInfo msaaState = {};
        createInfo.pMultisampleState = &msaaState;

        msaaState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        msaaState.rasterizationSamples = toVk(msaa.getSamples());
        msaaState.sampleShadingEnable = msaa.isSampleShading();

        // Shader reflection for Layout and Descriptors
        auto &shaderState = desc->getShader();
        auto &shaders = shaderState.shaders();

        std::map<uint32_t, std::map<uint32_t, DescriptorBindingInfo>> bindingsBySet;
        Vector<VkPushConstantRange> pushConstantRanges;
        uint32_t maxSet = 3; // Ensure sets 0..3 are covered for compatibility with standard engine passes

        for (const auto &shader : shaders | std::views::values) {
            auto compiled = shader->getCompiledShader();
            if (!compiled) continue;

            VkShaderStageFlags stageFlag = toVk(compiled->stage);

            for (const auto &u : compiled->uniforms) {
                if (u.set > maxSet) {
                    maxSet = u.set;
                }

                auto &bInfo = bindingsBySet[u.set][u.binding];
                bInfo.binding = u.binding;
                bInfo.resourceType = u.resourceType;
                bInfo.count = std::max(1u, u.getArraySize());
                bInfo.stageFlags |= stageFlag;
                bInfo.size = std::max(bInfo.size, u.size);
                bInfo.name = u.name;
            }

            for (const auto &pc : compiled->pushConstants) {
                VkPushConstantRange range{};
                range.stageFlags = toVk(pc.stage);
                range.offset = pc.offset;
                range.size = pc.size;
                pushConstantRanges.push_back(range);
            }
        }

        Vector<VkDescriptorSetLayout> descriptorLayouts;
        descriptorLayouts.resize(maxSet + 1);

        Vector<DescriptorSetInfo> reflectedSetsInfo;

        for (uint32_t s = 0; s <= maxSet; ++s) {
            if (s == SAMPLER_SET) {
                descriptorLayouts[s] = mSamplerDescSet->getLayouts();
            } else if (s == CAMERA_MATRICES_SET) {
                descriptorLayouts[s] = mCameraDescSet->getLayouts();
            } else if (s == MODEL_MATRICES_SET) {
                auto it = bindingsBySet.find(s);
                if (it == bindingsBySet.end() || (it->second.size() == 1 && it->second.count(MODEL_MATRICES_BINDING))) {
                    descriptorLayouts[s] = mModelDescSet->getLayouts();
                } else {
                    Vector<DescriptorBindingInfo> setBindings;
                    for (const auto &[bIdx, bInfo] : it->second) {
                        auto dynamicInfo = bInfo;
                        if (dynamicInfo.resourceType == ShaderResourceType::UNIFORM_BUFFER) {
                            dynamicInfo.resourceType = ShaderResourceType::UNIFORM_BUFFER_DYNAMIC;
                        }
                        setBindings.push_back(dynamicInfo);
                    }
                    descriptorLayouts[s] = getOrCreateDescriptorSetLayout(setBindings);
                }
            } else {
                auto it = bindingsBySet.find(s);
                if (it != bindingsBySet.end() && !it->second.empty()) {
                    Vector<DescriptorBindingInfo> setBindings;
                    for (const auto &[bIdx, bInfo] : it->second) {
                        setBindings.push_back(bInfo);
                    }
                    descriptorLayouts[s] = getOrCreateDescriptorSetLayout(setBindings);
                } else {
                    descriptorLayouts[s] = mEmptyDescriptorSetLayout;
                }
            }

            auto setIt = bindingsBySet.find(s);
            if (setIt != bindingsBySet.end() && !setIt->second.empty()) {
                DescriptorSetInfo setInfo{};
                setInfo.set = s;
                for (const auto &[bIdx, bInfo] : setIt->second) {
                    auto bindingInfo = bInfo;
                    if (s == MODEL_MATRICES_SET && bindingInfo.resourceType == ShaderResourceType::UNIFORM_BUFFER) {
                        bindingInfo.resourceType = ShaderResourceType::UNIFORM_BUFFER_DYNAMIC;
                    }
                    setInfo.bindings.push_back(bindingInfo);
                }
                reflectedSetsInfo.push_back(setInfo);
            }
        }

        // Pipeline Layout
        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size());
        layoutInfo.pSetLayouts = descriptorLayouts.data();
        layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
        layoutInfo.pPushConstantRanges = pushConstantRanges.empty() ? nullptr : pushConstantRanges.data();
        vkCreatePipelineLayout(*mDevice, &layoutInfo, nullptr, &createInfo.layout);

        // Rasterizer
        auto &rasterizer = desc->getRasterizer();
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

        // Dynamic Rendering
        auto &rendering = desc->getRenderingState();
        VkPipelineRenderingCreateInfo renderingInfo = {};
        createInfo.pNext = &renderingInfo;

        renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

        auto formats = rendering.getColorFormats();
        VkFormat vkFormats[formats.empty() ? 1 : formats.size()];
        renderingInfo.colorAttachmentCount = formats.size();
        renderingInfo.pColorAttachmentFormats = formats.empty() ? nullptr : vkFormats;

        for (int i = 0; i < formats.size(); i++) {
            vkFormats[i] = toVk(formats[i]);
        }

        renderingInfo.depthAttachmentFormat = toVk(rendering.getDepthFormat());
        renderingInfo.stencilAttachmentFormat = toVk(rendering.getStencilFormat());

        // Depth Stencil
        auto &depthState = desc->getDepth();
        VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        bool hasDepth = rendering.getDepthFormat() != Format::UNDEFINED;
        depthStencilState.depthTestEnable = (depthState.isDepthTestEnable() && hasDepth) ? VK_TRUE : VK_FALSE;
        depthStencilState.depthWriteEnable = (depthState.isDepthWriteEnable() && hasDepth) ? VK_TRUE : VK_FALSE;
        depthStencilState.depthCompareOp = toVk(depthState.getDepthCompareOp());
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.stencilTestEnable = VK_FALSE;
        depthStencilState.minDepthBounds = 0.0f;
        depthStencilState.maxDepthBounds = 1.0f;
        createInfo.pDepthStencilState = &depthStencilState;

        // Shader Stages
        VkPipelineShaderStageCreateInfo stages[shaders.size() == 0 ? 1 : shaders.size()];
        createInfo.stageCount = shaders.size();
        createInfo.pStages = stages;

        for (const auto &[index, shader]: shaders | std::views::values | std::views::enumerate) {
            auto &stage = stages[index];

            stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage = toVk(shader->getStage());
            stage.pName = "main";
            stage.flags = 0;
            stage.module = createShaderModule(*mDevice, shader->getCompiledShader()->bin);
            stage.pSpecializationInfo = nullptr;
            stage.pNext = nullptr;
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        createInfo.pVertexInputState = &vertexInputInfo;

        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto vertexShader = shaders.at(ShaderStage::VERTEX);
        auto &inputs = vertexShader->getCompiledShader()->inputs;

        VkVertexInputAttributeDescription attributes[inputs.size() == 0 ? 1 : inputs.size()];

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
        vertexInputInfo.vertexAttributeDescriptionCount = inputs.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributes;

        // Assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        createInfo.pInputAssemblyState = &inputAssembly;

        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = toVk(desc->getTopology());
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // RenderPass
        createInfo.renderPass = VK_NULL_HANDLE;
        createInfo.subpass = 0;

        // Dynamic state
        VkPipelineDynamicStateCreateInfo dynamicState{};
        createInfo.pDynamicState = &dynamicState;
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

        // Viewport
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = nullptr;
        viewportState.scissorCount = 1;
        viewportState.pScissors = nullptr;

        createInfo.pViewportState = &viewportState;
        return mDevice->createHeap<VulkanPipelineState>(*desc, createInfo, std::move(reflectedSetsInfo), descriptorLayouts);
    }

    void VulkanRenderResourceManager::updateSampler(Ref<Texture> texture) {
        mSamplerDescSet->update(*mDevice, SAMPLER_BINDING, *ResourceCast(texture));
    }

    Ref<BindGroup> VulkanRenderResourceManager::createBindGroup(Ref<PipelineState> pso, uint32_t setId) {
        auto vkPso = ResourceCast(pso);
        if (!vkPso) return nullptr;

        VkDescriptorSetLayout layout = vkPso->getDescriptorSetLayout(setId);
        if (layout == VK_NULL_HANDLE) {
            LOG_ERROR("Cannot create bind group: no descriptor layout for set {}", setId);
            return nullptr;
        }

        auto pool = getOrCreateDescriptorPool();
        auto set = mDevice->createHeap<VulkanDescriptorSet>(pool, layout);
        return new VulkanBindGroup(set, static_cast<uint8_t>(setId));
    }

    Ref<BindGroup> VulkanRenderResourceManager::createUniformBindGroup(Ref<PipelineState> pso, uint32_t setId, uint32_t binding, Ref<UniformBuffer> buffer) {
        auto group = createBindGroup(pso, setId);
        if (group && buffer) {
            updateBindGroup(group, binding, buffer);
        }
        return group;
    }

    Ref<BindGroup> VulkanRenderResourceManager::createTextureBindGroup(Ref<PipelineState> pso, uint32_t setId, uint32_t binding, Ref<Texture> texture) {
        auto group = createBindGroup(pso, setId);
        if (group && texture) {
            updateBindGroup(group, binding, texture);
        }
        return group;
    }

    void VulkanRenderResourceManager::updateBindGroup(Ref<BindGroup> bindGroup, uint32_t binding, Ref<UniformBuffer> buffer) {
        auto vkGroup = ResourceCast(bindGroup);
        if (vkGroup && vkGroup->getSet() && buffer) {
            vkGroup->getSet()->update(*mDevice, binding, *ResourceCast(buffer));
        }
    }

    void VulkanRenderResourceManager::updateBindGroupDynamic(Ref<BindGroup> bindGroup, uint32_t binding, Ref<UniformBuffer> buffer, size_t range) {
        auto vkGroup = ResourceCast(bindGroup);
        if (vkGroup && vkGroup->getSet() && buffer) {
            vkGroup->getSet()->updateDynamic(*mDevice, binding, *ResourceCast(buffer), range);
        }
    }

    void VulkanRenderResourceManager::updateBindGroup(Ref<BindGroup> bindGroup, uint32_t binding, Ref<Texture> texture) {
        auto vkGroup = ResourceCast(bindGroup);
        if (vkGroup && vkGroup->getSet() && texture) {
            vkGroup->getSet()->update(*mDevice, binding, *ResourceCast(texture));
        }
    }

    void VulkanRenderResourceManager::prepareCache() {
        VkDeviceSize minAlign = mDevice->getPhysicalDevice().getMinUniformBufferOffsetAlignment();
        mModelAlignment = sizeof(glm::mat4);
        if (minAlign > 0) {
            mModelAlignment = (mModelAlignment + minAlign - 1) & ~(minAlign - 1);
            mUniformAllocator.setAlignment(minAlign);
        }

        constexpr size_t MAX_MODELS = 4096;
        VkDeviceSize modelBufferSize = MAX_MODELS * mModelAlignment;

        // Create empty descriptor set layout (cached and reused for unused sets)
        if (mEmptyDescriptorSetLayout == VK_NULL_HANDLE) {
            VkDescriptorSetLayoutCreateInfo emptyInfo{};
            emptyInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            emptyInfo.bindingCount = 0;
            VK_CHECK(vkCreateDescriptorSetLayout(*mDevice, &emptyInfo, nullptr, &mEmptyDescriptorSetLayout), "Failed to create empty descriptor set layout");
        }

        auto pool = getOrCreateDescriptorPool();

        // Set 2: Camera
        {
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = 1;
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = 0;
            binding.descriptorCount = 1;
            binding.descriptorType = toVk(ShaderResourceType::UNIFORM_BUFFER);
            binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            createInfo.pBindings = &binding;

            VkDescriptorSetLayout layout;
            VK_CHECK(vkCreateDescriptorSetLayout(*mDevice, &createInfo, nullptr, &layout), "Failed to create camera descriptor layout");
            mCameraDescSet = mDevice->createHeap<VulkanDescriptorSet>(pool, layout);
            mCameraBuffer = allocateUniformBuffer(CAMERA_MATRICES_SIZE);
            mCameraDescSet->update(*mDevice, CAMERA_MATRICES_BINDING, *ResourceCast(mCameraBuffer));
            mCameraGroup = new VulkanBindGroup(mCameraDescSet, CAMERA_MATRICES_SET);
        }

        // Set 3: Model matrices (Dynamic uniform buffer)
        {
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = 1;
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = 0;
            binding.descriptorCount = 1;
            binding.descriptorType = toVk(ShaderResourceType::UNIFORM_BUFFER_DYNAMIC);
            binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            createInfo.pBindings = &binding;

            VkDescriptorSetLayout layout;
            VK_CHECK(vkCreateDescriptorSetLayout(*mDevice, &createInfo, nullptr, &layout), "Failed to create model descriptor layout");

            mModelDescSet = mDevice->createHeap<VulkanDescriptorSet>(pool, layout);
            mModelBuffer = allocateUniformBuffer(modelBufferSize);
            mModelDescSet->updateDynamic(*mDevice, MODEL_MATRICES_BINDING, *ResourceCast(mModelBuffer), sizeof(glm::mat4));
            mModelGroup = new VulkanBindGroup(mModelDescSet, MODEL_MATRICES_SET);
        }

        // Set 1: Sampler
        {
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = 1;

            VkDescriptorSetLayoutBinding binding{};
            binding.binding = 0;
            binding.descriptorCount = 1;
            binding.descriptorType = toVk(ShaderResourceType::COMBINED_IMAGE_SAMPLER);
            binding.pImmutableSamplers = nullptr;
            binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            createInfo.pBindings = &binding;

            VkDescriptorSetLayout layout;
            VK_CHECK(vkCreateDescriptorSetLayout(*mDevice, &createInfo, nullptr, &layout), "Failed to create sampler descriptor layout");
            mSamplerDescSet = mDevice->createHeap<VulkanDescriptorSet>(pool, layout);
            mSamplerGroup = new VulkanBindGroup(mSamplerDescSet, SAMPLER_SET);
        }

        mCachePrepared = true;
    }

NS_END
