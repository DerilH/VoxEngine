//
// Created by deril on 2/13/26.
//

#include <GLFW/glfw3.h>
#include <VoxEngine/render/windowing/Window.h>

namespace Vox::Render::Windowing {
    bool Window::sGlfwInitialized = false;

    Window::Window(std::string title, const int width, const int height) : mTitle(std::move(title)), mWidth(width),
                                                                           mHeight(height) {
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
        InitGlfw();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        LOG_ERROR("{}", glfwGetPlatform());

        mHandle = glfwCreateWindow(mWidth, mHeight, title.c_str(), nullptr, nullptr);


        glfwSetWindowUserPointer(mHandle, this);
        glfwSetWindowSizeCallback(mHandle, [](GLFWwindow *p, const int w, const int h) {
            auto *window = static_cast<Window *>(glfwGetWindowUserPointer(p));
            window->mWidth = w;
            window->mHeight = h;
            LOG_VERBOSE("Resized: w:{}, h:{}", w, h);
        });
    }

    int Window::getWidth() const {
        return mWidth;
    }

    int Window::getHeight() const {
        return mHeight;
    }

    std::string Window::getTitle() {
            return mTitle;
    }

    void Window::setTitle(std::string title) {
        mTitle = std::move(title);
        glfwSetWindowTitle(mHandle, mTitle.c_str());
    }

    void Window::resize(const int width, const int height) {
        mWidth = width;
        mHeight = height;
        glfwSetWindowSize(mHandle, mWidth, mHeight);
    }

    GLFWwindow *Window::getHandle() const {
        return mHandle;
    }

    bool Window::shouldClose() const {
            return glfwWindowShouldClose(mHandle);
    }

    void Window::makeCurrent() const {
        glfwMakeContextCurrent(mHandle);
    }

    void Window::destroy() const {
        glfwDestroyWindow(mHandle);
    }

    void Window::InitGlfw() {
        if (!sGlfwInitialized) {
            glfwInit();
            sGlfwInitialized = true;
        }
    }

    void Window::terminateGLfw() {
        if (sGlfwInitialized) {
            glfwTerminate();
            sGlfwInitialized = false;
        }
    }
}
