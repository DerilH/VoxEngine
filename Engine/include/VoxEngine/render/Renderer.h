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
        std::function<void(Vulkan::FrameSync)> mGui;

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
        virtual void addRenderTarget(RenderTarget* target) {
            mRenderTargets.push_back(target);
        }
        void stop() {
            mShouldStop = true;
        }

        void setGui(std::function<void(Render::Vulkan::FrameSync)> foo) {
            mGui = foo;
        }
    };

NS_END
