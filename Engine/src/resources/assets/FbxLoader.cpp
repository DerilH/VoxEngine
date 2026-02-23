//
// Created by deril on 2/19/26.
//

#include "VoxEngine/resources/assets/FbxLoader.h"
#include "VoxEngine/resources/assets/MeshAsset.h"
#include "VoxEngine/resources/assets/ModelAsset.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <vector>

RESOURCES_NS
    std::vector<glm::vec3> readVec3Array(aiVector3D *vertices, size_t count) {
        std::vector<glm::vec3> vec(count);
        for (int i = 0; i < count; ++i) {
            vec.at(i) = glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z);
        }

        return std::move(vec);
    }

    std::vector<glm::vec2> readVec2Array(aiVector3D *vertices, size_t count) {
        std::vector<glm::vec2> vec(count);
        for (int i = 0; i < count; ++i) {
            vec.at(i) = glm::vec2(vertices[i].x, vertices[i].y);
        }

        return std::move(vec);
    }

    std::vector<uint32_t> readIndices(aiFace *faces, size_t count, bool triangulated) {
        std::vector<uint32_t> vec(count * 3);
        if (triangulated) {
            for (int i = 0; i < count; ++i) {
                aiFace &face = faces[i];
                memcpy(((uint32_t *) vec.data()) + i * 3, (const void *) face.mIndices, face.mNumIndices * sizeof(uint32_t));
            }
        } else {
            for (int f = 0; f < count; ++f) {
                aiFace &face = faces[f];
                for (int i = 0; i < face.mNumIndices; i++) {
                    vec.at(i) = face.mIndices[i];
                }
            }
        }
        return std::move(vec);
    }

    Asset *FbxLoader::load(std::string path, void *data, size_t dataSize) {
        Assimp::Importer importer{};

        const aiScene *scene = importer.ReadFileFromMemory(data, dataSize, aiPostProcessSteps::aiProcess_Triangulate | aiPostProcessSteps::aiProcess_FlipUVs, "fbx");
        auto **nested = static_cast<Asset **>(malloc(scene->mNumMeshes * sizeof(MeshAsset)));

        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[i];
            auto vertices = readVec3Array(mesh->mVertices, mesh->mNumVertices);
            auto normals = readVec3Array(mesh->mNormals, mesh->mNumVertices);
            auto uvs = readVec2Array(mesh->mTextureCoords[0], mesh->mNumVertices);
            auto indices = readIndices(mesh->mFaces, mesh->mNumFaces, true);
            nested[i] = new MeshAsset(mesh->mName.C_Str(), vertices, normals, uvs, indices);
            LOG_VERBOSE("Mesh loaded {}", mesh->mName.C_Str());
        }
        LOG_VERBOSE("Model loaded {}", path);
        return new ModelAsset(path, nested, scene->mNumMeshes);
    }
NS_END

