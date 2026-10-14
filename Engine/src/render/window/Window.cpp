//
// Created by deril on 2/13/26.
//

#include <GLFW/glfw3.h>
#include <VoxEngine/render/windowing/Window.h>

namespace Vox::Render {
    Window::Window(InternedString title, Extent size, GLFWwindow *handle) : mHandle(handle), mTitle(std::move(title)), mExtent(size) {
        glfwSetWindowUserPointer(handle, this);
        glfwSetWindowSizeCallback(handle, [](GLFWwindow *p, const int w, const int h) {
            auto *window = static_cast<Window *>(glfwGetWindowUserPointer(p));
            window->mExtent.width = w;
            window->mExtent.height = h;
            window->onResize.invoke(window->mExtent);
            LOG_VERBOSE("Resized: w:{}, h:{}", w, h);
        });
    }

    Window *Window::Create(InternedString title, Extent size, int targetMonitorIndex, bool maximized) {
        int monitorCount = 0;
        GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
        VOX_CHECK(monitorCount > 0, "Cannot create window, no monitors found");
        if (monitorCount < targetMonitorIndex) {
            LOG_WARN("Monitor with by index {} not found, fallback to first found", targetMonitorIndex)
            targetMonitorIndex = 0;
        }
        GLFWmonitor *targetMonitor = monitors[targetMonitorIndex];


        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        const auto handle = glfwCreateWindow(size.width, size.height, title.c_str(), nullptr, nullptr);
        VOX_CHECK(handle, "Unknown error when creating window");

        if (maximized) {
            int monitorX, monitorY;
            glfwGetMonitorPos(targetMonitor, &monitorX, &monitorY);
            glfwSetWindowPos(handle, monitorX + 100, monitorY + 100);
            glfwMaximizeWindow(handle);

            glfwGetMonitorWorkarea(targetMonitor, nullptr, nullptr, reinterpret_cast<int*>(&size.width), reinterpret_cast<int*>(&size.height));
        }
        glfwSetWindowCloseCallback(handle, [](GLFWwindow* window) {
            auto w = (Window*)glfwGetWindowUserPointer(window);
            w->onClose.invoke(w);
        });
        return new Window(title, size, handle);
    }

    std::string Window::getTitle() {
        return mTitle;
    }

    void Window::setTitle(std::string title) {
        mTitle = std::move(title);
        glfwSetWindowTitle(mHandle, mTitle.c_str());
    }

    void Window::resize(const int width, const int height) {
        mExtent.width = width;
        mExtent.height = height;
        glfwSetWindowSize(mHandle, mExtent.width, mExtent.height);
    }

    void *Window::getHandle() const {
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
}
