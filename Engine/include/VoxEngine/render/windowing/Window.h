#pragma once
#include <string>
#include <GLFW/glfw3.h>
#include <VoxEngine/render/Renderer.h>

namespace Vox::Render::Windowing {
    class Window {
        static bool sGlfwInitialized;

        GLFWwindow* mHandle;
        std::string mTitle;
        int mWidth;
        int mHeight;

    public:
        Window(std::string title, int width, int height);

        int getWidth() const;
        int getHeight() const;
        std::string getTitle();

        void setTitle(std::string title);
        void resize(int width, int height);

        GLFWwindow* getHandle() const;
        bool shouldClose() const;
        void makeCurrent() const;
        void destroy() const;

        static void InitGlfw();
        static void terminateGLfw();
    };
}
