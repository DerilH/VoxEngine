#include <VoxEngine/render/windowing/WindowManager.h>

RENDER_NS
    bool WindowManager::sGlfwInitialized = false;


    Window *WindowManager::createWindow(InternedString title, Extent size, int targetMonitorIndex, bool maximized) {
        VOX_CHECK(sGlfwInitialized, "Windowing backend is not initialized");
        auto window = Window::Create(title, size, targetMonitorIndex, maximized);
        mWindows.emplace_back(window);
        return window;
    }

    WindowManager::~WindowManager() {
        terminateBackend();
    }

    void WindowManager::initBackend() {
        if (!sGlfwInitialized) {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
            glfwInit();
            sGlfwInitialized = true;
        }
    }

    void WindowManager::terminateBackend() {
        if (sGlfwInitialized) {
            glfwTerminate();
            sGlfwInitialized = false;
        }
    }

NS_END
