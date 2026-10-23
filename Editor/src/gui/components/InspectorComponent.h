#pragma once
#include <VoxCore/math/Types.h>
#include <VoxCore/math/UUID.h>
#include <VoxEngine/Engine.h>
#include <VoxEngine/render/RenderContext.h>
#include <VoxEngine/scene/GameObject.h>
#include "AssetLinkComponent.h"
#include "GuiComponent.h"

namespace Vox::Editor {
    class InspectorComponent : public GuiComponent {
        Ref<Scene::GameObject> mGameObject = nullptr;
        Vec3 mMeshRotation{};
        Ref<Engine> mEngine;

        UPtr<AssetLinkComponent> mMaterialLink;
        UPtr<AssetLinkComponent> mTextureLink;
        UPtr<AssetLinkComponent> mMeshLink;

        void renderComponents(Render::RenderContext context);
        void renderTransformOptions();

    protected:
        void renderInternal(Render::RenderContext context) override;

    public:
        Event<InternedString> onOpenAsset;
        Event<UUID> onOpenAssetUuid;

        explicit InspectorComponent(Ref<Engine> engine);

        void setObject(Ref<Scene::GameObject> gameObject);
        NO_COPY_MOVE(InspectorComponent)
    };
}
