#include "load.h"
#include <stdexcept>
#include <filesystem>
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
Load::Load() : scene_(nullptr) {}
bool Load::loadFromFile(const std::string& path) {
    path_ = path;
    clear();
    unsigned int flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices;
    scene_ = importer_.ReadFile(path, flags);
    if (!scene_) {
        error_ = importer_.GetErrorString();
        return false;
    }
    extractVertices(scene_);
    extractMaterials(scene_);
    std::string baseDir = std::filesystem::path(path).parent_path().string();
    extractTextures(scene_, baseDir);
    return true;
}
const std::vector<Vertex>& Load::vertices() const { return vertices_; }
const std::vector<MaterialDesc>& Load::materials() const { return materials_; }
const std::vector<TextureDesc>& Load::textures() const { return textures_; }
const std::string& Load::getPath() const { return path_; }
const char* Load::getError() const { return error_.empty() ? nullptr : error_.c_str(); }
void Load::clear() {
    vertices_.clear();
    materials_.clear();
    textures_.clear();
    error_.clear();
}
void Load::extractVertices(const aiScene* scene) {
    if (!scene) return;
    for (unsigned int mi = 0; mi < scene->mNumMeshes; ++mi) {
        const aiMesh* mesh = scene->mMeshes[mi];
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            Vertex v{};
            aiVector3D p = mesh->mVertices ? mesh->mVertices[i] : aiVector3D(0,0,0);
            aiVector3D n = mesh->mNormals ? mesh->mNormals[i] : aiVector3D(0,0,0);
            v.position[0]=p.x; v.position[1]=p.y; v.position[2]=p.z;
            v.normal[0]=n.x; v.normal[1]=n.y; v.normal[2]=n.z;
            if (mesh->mTextureCoords[0]) { v.uv0[0]=mesh->mTextureCoords[0][i].x; v.uv0[1]=mesh->mTextureCoords[0][i].y; } else { v.uv0[0]=0; v.uv0[1]=0; }
            if (mesh->mTextureCoords[1]) { v.uv1[0]=mesh->mTextureCoords[1][i].x; v.uv1[1]=mesh->mTextureCoords[1][i].y; } else { v.uv1[0]=0; v.uv1[1]=0; }
            if (mesh->mTextureCoords[2]) { v.uv2[0]=mesh->mTextureCoords[2][i].x; v.uv2[1]=mesh->mTextureCoords[2][i].y; } else { v.uv2[0]=0; v.uv2[1]=0; }
            vertices_.push_back(v);
        }
    }
}
void Load::extractMaterials(const aiScene* scene) {
    if (!scene) return;
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        const aiMaterial* m = scene->mMaterials[i];
        MaterialDesc md{};
        aiColor3D c(1.0f,1.0f,1.0f);
        if (AI_SUCCESS == m->Get(AI_MATKEY_COLOR_DIFFUSE, c)) {
            md.color[0]=c.r; md.color[1]=c.g; md.color[2]=c.b;
        } else {
            md.color[0]=1.0f; md.color[1]=1.0f; md.color[2]=1.0f;
        }
        aiString texPath;
        if (AI_SUCCESS == m->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)) {
            md.map_uuid = texPath.C_Str();
        }
        materials_.push_back(md);
    }
}
static std::string guessMime(const aiTexture* t) {
    std::string hint = t->achFormatHint[0] ? std::string(t->achFormatHint) : std::string();
    if (!hint.empty()) return std::string("image/") + hint;
    return std::string();
}
void Load::extractTextures(const aiScene* scene, const std::string& baseDir) {
    if (!scene) return;
    for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
        const aiTexture* t = scene->mTextures[i];
        TextureDesc td{};
        td.image_uuid = t->mFilename.length ? std::string(t->mFilename.C_Str()) : std::string("*") + std::to_string(i);
        td.width = static_cast<int>(t->mWidth);
        td.height = static_cast<int>(t->mHeight);
        td.mime_type = guessMime(t);
        td.wrap_s = 1000;
        td.wrap_t = 1000;
        td.min_filter = 1008;
        td.mag_filter = 1006;
        td.anisotropy = 1;
        td.srgb = true;
        td.flip_y = false;
        td.generate_mipmaps = true;
        if (t->mHeight == 0) {
            td.image_bytes.resize(t->mWidth);
            std::memcpy(td.image_bytes.data(), t->pcData, t->mWidth);
        } else {
            size_t byteCount = static_cast<size_t>(t->mWidth) * static_cast<size_t>(t->mHeight) * sizeof(aiTexel);
            td.image_bytes.resize(byteCount);
            std::memcpy(td.image_bytes.data(), t->pcData, byteCount);
        }
        textures_.push_back(std::move(td));
    }
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        const aiMaterial* m = scene->mMaterials[i];
        aiString texPath;
        if (AI_SUCCESS == m->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)) {
            std::string p = texPath.C_Str();
            if (!p.empty() && p[0] != '*') {
                std::filesystem::path full = std::filesystem::path(baseDir) / p;
                int w=0,h=0,ch=0;
                unsigned char* data = stbi_load(full.string().c_str(), &w, &h, &ch, 4);
                if (data) {
                    TextureDesc td{};
                    td.image_uuid = p;
                    td.width = w;
                    td.height = h;
                    td.mime_type = "image/unknown";
                    td.wrap_s = 1000;
                    td.wrap_t = 1000;
                    td.min_filter = 1008;
                    td.mag_filter = 1006;
                    td.anisotropy = 1;
                    td.srgb = true;
                    td.flip_y = false;
                    td.generate_mipmaps = true;
                    td.image_bytes.assign(data, data + static_cast<size_t>(w*h*4));
                    stbi_image_free(data);
                    textures_.push_back(std::move(td));
                }
            }
        }
    }
}
