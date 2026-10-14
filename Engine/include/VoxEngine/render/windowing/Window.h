#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/signal/Event.h>
#include <VoxEngine/input/Key.h>

#include "VoxCore/math/Extent.h"

namespace Vox::Render {
    class Window {
        friend class WindowManager;

        GLFWwindow* mHandle;
        InternedString mTitle;
        Extent mExtent;
        Window(InternedString title, Extent size, GLFWwindow* handle);
        static Window* Create(InternedString title, Extent size, int targetMonitorIndex, bool maximized);
    public:
        Event<Ref<Window>> onClose;
        Event<Extent> onResize;
        Event<Ref<Window>, Input::Key, Input::KeyAction> onKeyPressed;

        Extent getSize() const {
            return mExtent;
        }
        uint32_t getWidth() const {
            return mExtent.width;
        }
        uint32_t getHeight() const {
            return mExtent.height;
        }

        InternedString getTitle();

        void setTitle(std::string title);

        void resize(int width, int height);

        void* getHandle() const;

        bool shouldClose() const;

        void makeCurrent() const;

        void destroy() const;

        static void pollEvents() {
            glfwPollEvents();
        }
    };
}
