#include "load.h"
#include <stdexcept>
#include <filesystem>
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
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
const std::vector<GLuint>& Load::texturesGL() const { return textures_gl_; }
const std::string& Load::getPath() const { return path_; }
const char* Load::getError() const { return error_.empty() ? nullptr : error_.c_str(); }
void Load::clear() {
    vertices_.clear();
    materials_.clear();
    textures_gl_.clear();
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
static GLuint makeGLTextureRGBA8(const unsigned char* pixels, int w, int h, bool srgb, bool mipmaps) {
    GLuint tex = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    GLint internalFormat = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
    glTextureStorage2D(tex, 1, internalFormat, w, h);
    glTextureSubImage2D(tex, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (mipmaps) glGenerateTextureMipmap(tex);
    return tex;
}
void Load::extractTextures(const aiScene* scene, const std::string& baseDir) {
    if (!scene) return;
    for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
        const aiTexture* t = scene->mTextures[i];
        bool srgb = true;
        bool flip_y = false;
        bool mipmaps = true;
        if (t->mHeight == 0) {
            const unsigned char* mem = reinterpret_cast<const unsigned char*>(t->pcData);
            int w=0,h=0,channels=0;
            stbi_set_flip_vertically_on_load(flip_y);
            unsigned char* decoded = stbi_load_from_memory(mem, static_cast<int>(t->mWidth), &w, &h, &channels, 4);
            if (decoded) {
                GLuint g = makeGLTextureRGBA8(decoded, w, h, srgb, mipmaps);
                textures_gl_.push_back(g);
                stbi_image_free(decoded);
            }
        } else {
            GLuint tex = 0;
            glCreateTextures(GL_TEXTURE_2D, 1, &tex);
            GLint internalFormat = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
            glTextureStorage2D(tex, 1, internalFormat, static_cast<int>(t->mWidth), static_cast<int>(t->mHeight));
            glTextureSubImage2D(tex, 0, 0, 0, static_cast<int>(t->mWidth), static_cast<int>(t->mHeight), GL_BGRA, GL_UNSIGNED_BYTE, t->pcData);
            glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            if (mipmaps) glGenerateTextureMipmap(tex);
            textures_gl_.push_back(tex);
        }
    }
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        const aiMaterial* m = scene->mMaterials[i];
        aiString texPath;
        if (AI_SUCCESS == m->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)) {
            std::string p = texPath.C_Str();
            if (!p.empty() && p[0] != '*') {
                std::filesystem::path full = std::filesystem::path(baseDir) / p;
                int w=0,h=0,ch=0;
                stbi_set_flip_vertically_on_load(false);
                unsigned char* data = stbi_load(full.string().c_str(), &w, &h, &ch, 4);
                if (data) {
                    bool srgb = true;
                    bool mipmaps = true;
                    GLuint g = makeGLTextureRGBA8(data, w, h, srgb, mipmaps);
                    textures_gl_.push_back(g);
                    stbi_image_free(data);
                }
            }
        }
    }
}
