#include "SceneEditScreen.h"
#include "../components/DockPanelComponent.h"
#include <VoxCore/containers/Containers.h>

#include "../AssetExplorer.h"
#include "../components/HierarchyComponent.h"
#include "../components/InspectorComponent.h"
#include "../components/ViewportComponent.h"

namespace Vox::Editor {
    SceneEditScreen::SceneEditScreen(Ref<Engine> engine) : Screen(engine){
        auto hie = new HierarchyComponent(engine);
        auto inspector = new InspectorComponent(engine);
        auto assetExplorer = new AssetExplorer(engine);

        hie->onSelectedChanged += [this, inspector](auto obj) {
            mSelectedObject = obj;
            onSelectedChanged.invoke(obj);
            inspector->setObject(mSelectedObject);
        };

        inspector->onOpenAsset += [assetExplorer](InternedString path) {
            assetExplorer->openAsset(path);
        };

        engine->onSceneChanged += [hie](auto scene) {
            hie->setScene(scene);
        };

        mChilds.emplace("Hierarchy", hie);
        mChilds.emplace("Inspector", inspector);
        mChilds.emplace("Assets", assetExplorer);
        mChilds.emplace("Viewport", new ViewportComponent(engine));
    }

    void SceneEditScreen::renderInternal(Render::RenderContext context) {
    }
}
