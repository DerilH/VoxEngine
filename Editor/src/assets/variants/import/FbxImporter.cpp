//
// Created by deril on 2/19/26.
//

#include "FbxImporter.h"
#include "../MeshAsset.h"
#include "../ModelAsset.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <vector>

#include "../../AssetManager.h"

ASSETS_NS
    static std::vector<glm::vec3> readVec3Array(aiVector3D *vertices, size_t count) {
        std::vector<glm::vec3> vec(count);
        for (size_t i = 0; i < count; ++i) {
            vec.at(i) = glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z);
        }
        return vec;
    }

    static std::vector<glm::vec4> readVec4Array(aiColor4D *vertices, size_t count) {
        std::vector<glm::vec4> vec(count);
        for (size_t i = 0; i < count; ++i) {
            vec.at(i) = glm::vec4(vertices[i].r, vertices[i].g, vertices[i].b, vertices[i].a);
        }
        return vec;
    }

    static std::vector<glm::vec2> readVec2Array(aiVector3D *vertices, size_t count) {
        std::vector<glm::vec2> vec(count);
        for (size_t i = 0; i < count; ++i) {
            vec.at(i) = glm::vec2(vertices[i].x, vertices[i].y);
        }
        return vec;
    }

    static std::vector<uint32_t> readIndices(aiFace *faces, size_t count) {
        std::vector<uint32_t> vec;
        vec.reserve(count * 3);
        for (size_t f = 0; f < count; f++) {
            const aiFace &face = faces[f];
            for (size_t i = 0; i < face.mNumIndices; i++) {
                vec.push_back(face.mIndices[i]);
            }
        }
        return vec;
    }

    template<typename T>
    static std::vector<char8_t> createInterleaved(const std::vector<glm::vec3> &vertices, const std::vector<T> &attrs) {
        static_assert(std::is_same_v<typename T::value_type, float>, "Attribute must be a float glm vector");
        VOX_CHECK(vertices.size() == attrs.size(), "Different vertices and attribute count");

        constexpr size_t attrLen = T::length();
        constexpr size_t stride = (3 + attrLen) * sizeof(float);

        std::vector<char8_t> buffer(vertices.size() * stride);
        char8_t *dst = buffer.data();

        for (size_t i = 0; i < vertices.size(); ++i, dst += stride) {
            std::memcpy(dst, &vertices[i], sizeof(glm::vec3));
            std::memcpy(dst + sizeof(glm::vec3), &attrs[i], sizeof(T));
        }
        return buffer;
    }

    Ref<Asset> FbxImporter::load(FILE *file) {
        Assimp::Importer importer{};
        auto data = readFileData(file);

        const aiScene *scene = importer.ReadFileFromMemory(data.value().pData, data.value().size(), aiPostProcessSteps::aiProcess_Triangulate);
        VOX_CHECK_FMT(scene != nullptr, "Failed to load model: {}", importer.GetErrorString());

        auto **nested = static_cast<Asset **>(malloc(scene->mNumMeshes * sizeof(Asset *)));
        LOG_VERBOSE("Loading model {}", scene->mName.C_Str());
        LOG_VERBOSE("Loading {} meshes", scene->mNumMeshes);

        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[i];

            auto vertices = readVec3Array(mesh->mVertices, mesh->mNumVertices);
            auto normals = readVec3Array(mesh->mNormals, mesh->mNumVertices);
            auto indices = readIndices(mesh->mFaces, mesh->mNumFaces);
            auto interleaved = mesh->HasVertexColors(0)
                ? createInterleaved<glm::vec4>(vertices, readVec4Array(mesh->mColors[0], mesh->mNumVertices))
                : createInterleaved<glm::vec2>(vertices, readVec2Array(mesh->mTextureCoords[0], mesh->mNumVertices));

            // std::string meshPath = path.c_str();
            // meshPath += "/";
            // meshPath += mesh->mName.C_Str();

            nested[i] = new MeshAsset(interleaved, vertices, normals, indices);
            // LOG_VERBOSE("Mesh loaded {}", nested[i]->getPath().c_str());
        }
        return new ModelAsset(nested, scene->mNumMeshes);
    }
NS_END
