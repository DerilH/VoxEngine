//
// Created by deril on 2/19/26.
//

#include "../../../../Engine/include/VoxEngine/resources/Resource.h"

#include <VoxCore/containers/Containers.h>

RESOURCES_NS
    Resource::Resource(InternedString path, Resource **nested, uint32_t nestedCount) : mPath(path), mName(std::filesystem::path(path).filename()), mNestedResources(nested), mNestedResourcesCount(nestedCount) {
    }

    bool Resource::hasNested() const {
        return mNestedResources != nullptr;
    }


    uint32_t Resource::getNestedCount() const {
        return mNestedResourceCount;
    }

    UPtr<Resource> Resource::clone() const {
        VOX_CHECK_FMT(false, "{} Resource cannot be clone", +type())
    }

    void Resource::cloneTo(void *ptr) const {
        VOX_CHECK_FMT(false, "{} Resource cannot be clone", +type())
    }
    Resource::~Resource() = default;
NS_END
