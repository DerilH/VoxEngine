#pragma once
#include <atomic>
#include <VoxCore/Define.h>
#include <VoxCore/containers/Containers.h>

#include "GameObject.h"

SCENE_NS
    class GameObjectRegistry {
        HashMap<uint64_t, Ref<GameObject> > mMap;
        std::atomic<uint64_t> mNextObjectId{1};

    public:
        explicit GameObjectRegistry() = default;
        void add(uint64_t id, Ref<GameObject> n) { mMap.emplace(id, n); }
        void remove(uint64_t id) { mMap.erase(id); }


        Ref<GameObject> find(uint64_t id) const {
            auto it = mMap.find(id);
            return it == mMap.end() ? nullptr : it->second;
        }

        uint64_t allocGameObjectId() {
            return mNextObjectId.fetch_add(1, std::memory_order_relaxed);
        }

        NO_COPY_MOVE(GameObjectRegistry);
    };

NS_END
