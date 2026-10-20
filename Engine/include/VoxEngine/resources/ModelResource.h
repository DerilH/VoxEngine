//
// Created by deril on 2/20/26.
//

#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Assert.h>
#include <VoxCore/containers/Containers.h>
#include "ResourceType.h"
#include "ResourceHandle.h"
#include "MeshResource.h"

RESOURCES_NS
    class ModelResource {
        InternedString mName;
        Vector<MeshHandle> mMeshes;
        Vector<InternedString> mMeshNames;

    public:
        ModelResource() = default;

        ModelResource(InternedString name, Vector<MeshHandle> meshes, Vector<InternedString> meshNames = {})
            : mName(name), mMeshes(std::move(meshes)), mMeshNames(std::move(meshNames)) {}

        Expected<void> validate() const {
            VOX_EXPECT(!mMeshes.empty(), "ModelResource must contain at least one mesh handle");
            for (size_t i = 0; i < mMeshes.size(); ++i) {
                VOX_EXPECT(mMeshes[i].isValid(), "ModelResource mesh handle at index {} is invalid", i);
            }
            return {};
        }

        InternedString getName() const noexcept { return mName; }
        const Vector<MeshHandle> &getMeshes() const noexcept { return mMeshes; }
        uint32_t getMeshCount() const noexcept { return static_cast<uint32_t>(mMeshes.size()); }
        MeshHandle getMesh(uint32_t index) const {
            VOX_ASSERT(index < mMeshes.size(), "Mesh index out of bounds");
            return mMeshes[index];
        }
        const Vector<InternedString> &getMeshNames() const noexcept { return mMeshNames; }
    };

    template<>
    struct ResourceTraits<ModelResource> {
        static constexpr ResourceType Type = ResourceType::MODEL;
        static constexpr const char *Name = "Model";
    };

    using ModelHandle = ResourceHandle<ModelResource>;
NS_END
