//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxEngine/render/Backend.h"
#include "VoxEngine/render/Enums.h"

VULKAN_NS
class VulkanBackend : public Render::Backend {
        VkInstance mInstance = nullptr;

        void createInstance();

        void init() override;

        explicit VulkanBackend() = default;

    public:

        static Backend* Create() {
            return new VulkanBackend();
        }
    };

NS_END
