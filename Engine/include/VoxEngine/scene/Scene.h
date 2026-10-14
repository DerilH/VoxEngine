#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>

#include "Types.h"
#include "components/CameraComponent.h"
#include "components/RendererComponent.h"
SCENE_NS
    class Scene {
        Vector<GameObjectRef> mRootObjects;
        InternedString mName;
        HashSet<Ref<RendererComponent> > mRenderables;
        HashSet<Ref<CameraComponent> > mCameras;
        Ref<CameraComponent> mActiveCamera = nullptr;

    public:
        explicit Scene(InternedString name);

        GameObjectRef createObject(InternedString name);

        void removeObject(GameObjectRef gameObject);

        const Vector<GameObjectRef> &getRootObjects();

        void addRenderable(Ref<RendererComponent> renderable);

        void removeRenderable(Ref<RendererComponent> renderable);

        void addCamera(Ref<CameraComponent> camera);

        void removeCamera(Ref<CameraComponent> camera);

        const HashSet<Ref<RendererComponent> > &getAllRenderables() const {
            return mRenderables;
        }

        const HashSet<Ref<CameraComponent> > &getCameras() const {
            return mCameras;
        }

        void setActiveCamera(Ref<CameraComponent> camera) {
            auto a = mCameras.find(camera);
            if (a == mCameras.end()) {
                LOG_WARN("Trying to use not registered camera");
                return;
            }
            mActiveCamera = camera;
        }

        Ref<CameraComponent> getActiveCamera() const {
            return mActiveCamera;
        }

        NO_COPY_MOVE(Scene)
    };

NS_END
