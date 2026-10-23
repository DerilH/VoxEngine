#include "SceneEditScreen.h"
#include "../components/DockPanelComponent.h"
#include <VoxCore/containers/Containers.h>

#include "../AssetExplorer.h"
#include "../components/HierarchyComponent.h"
#include "../components/InspectorComponent.h"
#include "../components/ViewportComponent.h"

namespace Vox::Editor {
    void SceneEditScreen::renderInternal(Render::RenderContext context) {
    }

    SceneEditScreen::SceneEditScreen(Ref<Engine> engine, Ref<GizmoPass> gizmoPass) : Screen(engine) {
        auto hie = new HierarchyComponent(engine);
        auto inspector = new InspectorComponent(engine);
        auto assetExplorer = new AssetExplorer(engine);
        auto view = new ViewportComponent(engine, gizmoPass);

        hie->onSelectedChanged += [this, inspector, view](auto obj) {
            mSelectedObject = obj;
            onSelectedChanged.invoke(obj);
            inspector->setObject(mSelectedObject);
            view->setSelected(obj);
        };

        inspector->onOpenAsset += [assetExplorer](InternedString path) {
            assetExplorer->openAsset(path);
        };
        inspector->onOpenAssetUuid += [assetExplorer](UUID uuid) {
            assetExplorer->openAsset(uuid);
        };

        engine->onSceneChanged += [hie](auto scene) {
            hie->setScene(scene);
        };

        mChilds.emplace("Hierarchy", hie);
        mChilds.emplace("Inspector", inspector);
        mChilds.emplace("Assets", assetExplorer);
        mChilds.emplace("Viewport", view);
        view->onObjectPicked += [this, inspector, hie, view](Ref<Scene::GameObject> obj) {
            mSelectedObject = obj;
            onSelectedChanged.invoke(obj);
            hie->setSelected(obj);
            inspector->setObject(mSelectedObject);
            view->setSelected(obj);
        };
    }
}
