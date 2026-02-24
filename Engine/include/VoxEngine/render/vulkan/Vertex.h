//
// Created by deril on 2/14/26.
//

#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

namespace Vox::Render::Vulkan {
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static Buffer<VkVertexInputAttributeDescription> getAttributeDescriptions() {
            Buffer<VkVertexInputAttributeDescription> buff(new VkVertexInputAttributeDescription[2], 2);
            buff[0].binding = 0;
            buff[0].location = 0;
            buff[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            buff[0].offset = offsetof(Vertex, pos);

            buff[1].binding = 0;
            buff[1].location = 1;
            buff[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            buff[1].offset = offsetof(Vertex, color);
            return std::move(buff);
        }
    };
}
