#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include "VoxCore/math/Extent.h"

namespace Vox::Render::Windowing {
    class Window {
        static bool sGlfwInitialized;

        GLFWwindow* mHandle;
        std::string mTitle;
        Extent mExtent;
    public:
        Window(std::string title, int width, int height);

        Extent getExtent() const;
        uint32_t getWidth() const;
        uint32_t getHeight() const;

        std::string getTitle();

        void setTitle(std::string title);

        void resize(int width, int height);

        GLFWwindow* getHandle() const;

        bool shouldClose() const;

        void makeCurrent() const;

        void destroy() const;

        static void InitGlfw();

        static void terminateGLfw();

        static void pollEvents() {
            glfwPollEvents();
        }
    };
}
