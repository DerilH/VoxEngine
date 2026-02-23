//
// Created by deril on 2/17/26.
//

#pragma once
#include "RenderTarget.h"
#include "shaders/ShaderRepository.h"
#include "vulkan/VulkanWindowRenderTarget.h"

RENDER_NS
    typedef enum {
        VULKAN_API,
        OPENGL_API
    }
    RenderAPI;

    class Renderer {
        friend class RendererFactory;
    protected:
        std::vector<RenderTarget*> mRenderTargets;
        Shader::ShaderRepository &mShaderRepository;
        int mBufferingLevel = 0;
        bool mShouldStop = false;

        explicit Renderer(Shader::ShaderRepository &shaderRepository) : mShaderRepository(shaderRepository) {
        }

    public:
        Renderer(const Renderer &) = delete;

        Renderer(const Renderer &&) = delete;

        Renderer &operator=(const Renderer &) = delete;

        virtual ~Renderer() = default;

        virtual void init() = 0;

        virtual void renderLoop() = 0;

        virtual void setBuffering(char buffers) = 0;
        void addRenderTarget(RenderTarget* target) {
            mRenderTargets.push_back(target);
            ((Vulkan::VulkanWindowRenderTarget*)target)->addRenderPass(FORWARD_PASS);
        }
        void stop() {
            mShouldStop = true;
        }
    };

NS_END
