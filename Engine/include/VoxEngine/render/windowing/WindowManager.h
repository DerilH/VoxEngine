#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/SingletonBase.h>
#include <VoxEngine/render/vulkan/PhysicalDevice.h>

#include "Window.h"

RENDER_NS
    class WindowManager : public SingletonBase<WindowManager> {
        static bool sGlfwInitialized;

        Vector<UPtr<Window>> mWindows;
        Ref<Window> mMainWindow = nullptr;
    public:
        Window * createWindow(InternedString title, Extent size, int targetMonitorIndex, bool maximized);
        const Vector<UPtr<Window>>& getWindows() {
            return mWindows;
        }

        void setMainWindow(Ref<Window> window) {
            mMainWindow = window;
        }

        Ref<Window> getMainWindow() const {
            return mMainWindow;
        }

        ~WindowManager();

        static void initBackend();
        static void terminateBackend();
    };

NS_END
