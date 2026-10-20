//
// Created by deril on 2/20/26.
//

#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <VoxCore/Define.h>
#include <VoxCore/Assert.h>
#include <VoxCore/containers/Containers.h>
#include "ResourceType.h"
#include "ResourceHandle.h"

RESOURCES_NS
    class MeshResource {
        InternedString mName;
        std::vector<glm::vec3> mVertices;
        Vector<char8_t> mInterleaved;
        std::vector<glm::vec3> mNormals;
        std::vector<uint32_t> mIndices;

    public:
        MeshResource() = default;
        MeshResource(InternedString name,
                     Vector<char8_t> interleaved,
                     std::vector<glm::vec3> vertices,
                     std::vector<glm::vec3> normals,
                     std::vector<uint32_t> indices)
            : mName(name),
              mVertices(std::move(vertices)),
              mInterleaved(std::move(interleaved)),
              mNormals(std::move(normals)),
              mIndices(std::move(indices)) {}

        Expected<void> validate() const {
            VOX_EXPECT(!mVertices.empty(), "MeshResource has no vertices");
            VOX_EXPECT(!mIndices.empty(), "MeshResource has no indices");
            VOX_EXPECT(!mInterleaved.empty(), "MeshResource has no interleaved vertex data");
            const size_t vertexCount = mVertices.size();
            for (size_t i = 0; i < mIndices.size(); ++i) {
                VOX_EXPECT(mIndices[i] < vertexCount, "Index out of vertex bounds at {}: {}", i, mIndices[i]);
            }
            return {};
        }

        InternedString getName() const noexcept { return mName; }
        const std::vector<glm::vec3> &getVertices() const noexcept { return mVertices; }
        const Vector<char8_t> &getInterleaved() const noexcept { return mInterleaved; }
        const std::vector<glm::vec3> &getNormals() const noexcept { return mNormals; }
        const std::vector<uint32_t> &getIndices() const noexcept { return mIndices; }
        uint32_t getVertexCount() const noexcept { return static_cast<uint32_t>(mVertices.size()); }
        uint32_t getIndexCount() const noexcept { return static_cast<uint32_t>(mIndices.size()); }
    };

    template<>
    struct ResourceTraits<MeshResource> {
        static constexpr ResourceType Type = ResourceType::MESH;
        static constexpr const char *Name = "Mesh";
    };

    using MeshHandle = ResourceHandle<MeshResource>;
NS_END
