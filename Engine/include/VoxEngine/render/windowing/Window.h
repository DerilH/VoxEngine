#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <VoxCore/containers/Containers.h>

#include "VoxCore/math/Extent.h"

namespace Vox::Render {
    class Window {
        friend class WindowManager;

        GLFWwindow* mHandle;
        InternedString mTitle;
        Extent mExtent;
        std::function<void()> mOnClose;
        Window(InternedString title, Extent size, GLFWwindow* handle);
        static Window* Create(InternedString title, Extent size, int targetMonitorIndex, bool maximized);
    public:

        Extent getExtent() const;
        uint32_t getWidth() const;
        uint32_t getHeight() const;

        InternedString getTitle();

        void setTitle(std::string title);

        void resize(int width, int height);

        void* getHandle() const;

        bool shouldClose() const;

        void makeCurrent() const;

        void destroy() const;

        void setOnClose(std::function<void()> onClose);
        static void pollEvents() {
            glfwPollEvents();
        }
    };
}
