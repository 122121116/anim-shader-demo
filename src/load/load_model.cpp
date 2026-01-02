#include "loader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
MeshData Loader::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    MeshData out;
    if (!scene || !scene->HasMeshes()) return out;
    const aiMesh* mesh = scene->mMeshes[0];
    out.positions.reserve(mesh->mNumVertices);
    out.normals.reserve(mesh->mNumVertices);
    out.texcoords.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        aiVector3D p = mesh->mVertices[i];
        aiVector3D n = mesh->HasNormals() ? mesh->mNormals[i] : aiVector3D(0,0,0);
        aiVector3D t = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0,0,0);
        out.positions.push_back(glm::vec3(p.x, p.y, p.z));
        out.normals.push_back(glm::vec3(n.x, n.y, n.z));
        out.texcoords.push_back(glm::vec2(t.x, t.y));
    }
    for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
        const aiFace& face = mesh->mFaces[f];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) out.indices.push_back(face.mIndices[j]);
    }
    return out;
}
