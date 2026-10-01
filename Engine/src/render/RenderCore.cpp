#include "VoxCore/Define.h"
#include "VoxEngine/render/RenderCore.h"

#include <VoxEngine/render/vulkan/VulkanBackend.h>

RENDER_NS
    RenderBackend *CreateRenderBackend(RenderAPI api) {
        RenderBackend *back = nullptr;
        switch (api) {
            case VULKAN_API: {
                back = Vulkan::VulkanBackend::Create();
                break;
            }
            default:
                VOX_NO_IMPL("Unsupported render api");
        }
        back->init();
        return back;
    }
NS_END
