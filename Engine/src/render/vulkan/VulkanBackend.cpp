//
// Created by deril on 3/1/26.
//

#include "VoxEngine/render/vulkan/VulkanBackend.h"

#include <VoxEngine/render/TextureRenderTarget.h>
#include <VoxEngine/render/vulkan/VulkanTextureRenderTarget.h>

#include "VoxEngine/render/vulkan/Debug.h"
#include "VoxEngine/render/vulkan/VulkanDevice.h"
#include "VoxEngine/render/vulkan/VulkanResourceCast.h"
#include "VoxEngine/render/vulkan/VulkanUtil.h"
#include <VoxEngine/render/vulkan/buffers/VulkanUniformBuffer.h>
#include <vulkan/vulkan.h>

VULKAN_NS
    const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
    };

    void VulkanBackend::createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "N";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        //TODO: Add version choice
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

#ifdef VK_ENABLE_VALIDATION
        if (checkValidationLayerSupport()) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
        } else
            LOG_WARN("Validation layers requested but not supported");
#else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#endif

        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &mInstance), "Failed to create vulkan instance!");
    }

    void VulkanBackend::init() {
        VOX_CHECK(mInstance == nullptr, "Attempt to reinitialize render backend")
        createInstance();

        setupDebugMessenger(mInstance, mDebugMessenger);

        std::vector<std::string> extensions;
        for (const char *ext: deviceExtensions) {
            extensions.emplace_back(ext);
        }

        auto physicalDevices = PhysicalDevice::pickDevices(mInstance, extensions);
        mCurrentDevice = VulkanDevice::Create(mInstance, physicalDevices[0], deviceExtensions, VALIDATION_LAYERS);
    }

    VulkanBackend::VulkanBackend(RenderAPI api) : RenderBackend(api) {
    }

    int32_t VulkanBackend::beginFrame() {
        return 0;
    }

    void VulkanBackend::endFrame() {
    }

    RenderTargetRef VulkanBackend::createWindowTarget(Ref<Window> window) const {
        Surface *surface = Surface::Create(window, mInstance);
        surface->setDevice(ResourceCast(mCurrentDevice));
        return surface;
    }

    RenderTargetRef VulkanBackend::createTextureTarget(Extent extent, Format format, Format depthFormat) const {
        TextureRef depth = (depthFormat != Format::UNDEFINED) ? createTexture(depthFormat, extent) : nullptr;
        return new VulkanTextureRenderTarget(createTexture(format, extent), depth, ResourceCast(mCurrentDevice));
    }

    CommandPoolRef VulkanBackend::createCommandPool() {
        VOX_ASSERT(isInitialized(), "Render backend not initialized");
        auto device = ResourceCast(mCurrentDevice);
        auto queue = device->getQueue(QueueType::GRAPHICS_QUEUE);
        return device->createHeap<VulkanCommandPool>(queue.getFamily());
    }

    TextureRef VulkanBackend::createTexture(Format format, Extent extent) const {
        VOX_ASSERT(isInitialized(), "Render backend not initialized");
        const VulkanDevice *device = ResourceCast(mCurrentDevice);
        VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_2_TRANSFER_DST_BIT_KHR;
        if (isDepthFormat(format)) {
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        } else {
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        return device->createHeap<VulkanTexture>(format, extent, usage);
    }

    TextureRef VulkanBackend::createTexture(Format format, Extent extent, const void *data, uint32_t size) const {
        const VulkanDevice *device = ResourceCast(mCurrentDevice);
        auto staging = device->create<VulkanTransferBuffer>(size);

        const Queue &queue = device->getQueue(TRANSFER_QUEUE);
        CommandPool &pool = device->getCmdPool(TRANSFER_QUEUE);

        auto texture = createTexture(format, extent);

        VulkanCommandBuffer *cmdBuffer = ResourceCast(pool.allocBuffer());
        cmdBuffer->begin();
        staging.write(data, size);
        staging.copy(cmdBuffer, texture, size);
        cmdBuffer->end();
        queue.submit({*cmdBuffer}, true);
        return texture;
    }

    // PipelineStateRef VulkanBackend::createPSO(const PipelineStateDesc& desc) {
    // VOX_ASSERT(isInitialized(), "Render backend not initialized");
    // const VulkanDevice* device = ResourceCast(mCurrentDevice);
    // return device->createHeap<VulkanPipelineState>(desc);
    // }

    IndexBufferRef VulkanBackend::createIndexBuffer(const void *data, uint32_t size, IndexType type) {
        const VulkanDevice *device = ResourceCast(mCurrentDevice);
        auto staging = device->create<VulkanTransferBuffer>(size);

        const Queue &queue = device->getQueue(TRANSFER_QUEUE);
        CommandPool &pool = device->getCmdPool(TRANSFER_QUEUE);

        auto buffer = device->createHeap<VulkanIndexBuffer>(size, type);

        VulkanCommandBuffer *cmdBuffer = ResourceCast(pool.allocBuffer());
        cmdBuffer->begin();
        staging.write(data, size);
        staging.copy(cmdBuffer, buffer, size);
        cmdBuffer->end();
        queue.submit({*cmdBuffer}, true);
        return buffer;
    }

    VertexBufferRef VulkanBackend::createVertexBuffer(const void *data, uint32_t size, BufferUsage usage) {
        VulkanDevice *device = ResourceCast(mCurrentDevice);
        auto staging = device->create<VulkanTransferBuffer>(size);

        const Queue &queue = device->getQueue(TRANSFER_QUEUE);
        CommandPool &pool = device->getCmdPool(TRANSFER_QUEUE);

        auto buffer = device->createHeap<VulkanVertexBuffer>(size);

        VulkanCommandBuffer *cmdBuffer = ResourceCast(pool.allocBuffer());
        cmdBuffer->begin();
        staging.write(data, size);
        staging.copy(cmdBuffer, buffer, size);
        cmdBuffer->end();
        queue.submit({*cmdBuffer}, true);
        return buffer;
    }

    UniformBufferRef VulkanBackend::createUniformBuffer(uint32_t size) {
        VulkanDevice *device = ResourceCast(mCurrentDevice);
        return device->createHeap<VulkanUniformBuffer>(size);
    }
NS_END
