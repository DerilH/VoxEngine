#include "VoxCore/Define.h"
#include "VoxEngine/render/RenderCore.h"

#include <VoxEngine/render/RenderResourceManager.h>
#include <VoxEngine/render/vulkan/VulkanBackend.h>
#include <VoxEngine/render/vulkan/VulkanRenderResourceManager.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>

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

    Ref<RenderResourceManager> CreateRenderResourceManager(Ref<RenderBackend> back) {
        RenderResourceManager *res = nullptr;
        switch (back->api) {
            case VULKAN_API: {
                res = new Vulkan::VulkanRenderResourceManager(back, Vulkan::ResourceCast(back->getDevice()));
                break;
            }
            default:
                VOX_NO_IMPL("Unsupported render api");
        }
        return res;
    }

NS_END
