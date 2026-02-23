//
// Created by deril on 2/17/26.
//

#include <VoxEngine/render/RendererFactory.h>
#include <VoxEngine/render/vulkan/VulkanRenderer.h>
RENDER_NS
    Renderer* RendererFactory::Create(Shader::ShaderRepository& shaderRepository, const RenderAPI api) {
    switch (api) {
        case OPENGL_API:
            VOX_NO_IMPL("OpenGL renderer");

        case VULKAN_API:
            return new Vulkan::VulkanRenderer(shaderRepository);
    }
    return nullptr;
}

NS_END
