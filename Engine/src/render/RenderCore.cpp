#include "VoxCore/Define.h"
#include "VoxEngine/render/RenderCore.h"

RENDER_NS
    void InitRenderBackend(RenderAPI api) {
        switch (api) {
            case VULKAN_API: {
                RenderBackend::Init(Vulkan::VulkanBackend::Create());
                break;
            }
            default:
                VOX_NO_IMPL("Unsupported render api");
        }
}
NS_END