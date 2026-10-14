#pragma once
#include <VoxEngine/Engine.h>

#include "../components/GuiComponent.h"

namespace Vox::Editor {
    class Screen : public GuiComponent {
    protected:
        Ref<Engine> mEngine;
    public:
        explicit Screen(Ref<Engine> engine) : mEngine(engine) {}
        NO_COPY_MOVE_DEFAULT(Screen);
    };
}
