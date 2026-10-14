#pragma once
#include <VoxCore/containers/Containers.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/signal/Event.h>
#include <VoxEngine/render/RenderContext.h>

namespace Vox::Editor {
    class GuiComponent {
    protected:
        HashMap<InternedString, UPtr<GuiComponent> > mChilds;

        virtual void renderInternal(Render::RenderContext context) = 0;

    public:
        const Event<Ref<GuiComponent>> onClick;
        explicit GuiComponent() = default;

        virtual ~GuiComponent() = default;

        void render(Render::RenderContext context) {
            renderInternal(context);
            for (const auto &val: mChilds | std::views::values) {
                val->render(context);
            }
        }
        NO_COPY_MOVE(GuiComponent);
    };
}
