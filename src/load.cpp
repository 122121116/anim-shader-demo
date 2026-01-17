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
    textures_gl_ = extractTextures(scene_, baseDir);
    return true;
}
const std::vector<Vertex>& Load::vertices() const { return vertices_; }
const std::vector<MaterialDesc>& Load::materials() const { return materials_; }
const std::vector<TextureDesc>& Load::textures() const { return textures_; }
const std::vector<GLuint>& Load::texturesGL() const { return textures_gl_; }
const std::string& Load::getPath() const { return path_; }
const char* Load::getError() const { return error_.empty() ? nullptr : error_.c_str(); }
void Load::clear() {
    vertices_.clear();
    materials_.clear();
    textures_.clear();
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
std::vector<GLuint> Load::extractTextures(const aiScene* scene, const std::string& baseDir) {
    std::vector<GLuint> glTex;
    if (!scene) return;
    for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
        const aiTexture* t = scene->mTextures[i];
        TextureDesc td{};
        td.image_uuid = t->mFilename.length ? std::string(t->mFilename.C_Str()) : std::string("*") + std::to_string(i);
        td.width = static_cast<int>(t->mWidth);
        td.height = static_cast<int>(t->mHeight);
        td.mime_type = guessMime(t);
        td.wrap_s = 0;
        td.wrap_t = 0;
        td.min_filter = 0;
        td.mag_filter = 0;
        td.anisotropy = 1;
        td.srgb = true;
        td.flip_y = false;
        td.generate_mipmaps = true;
        if (t->mHeight == 0) {
            const unsigned char* mem = reinterpret_cast<const unsigned char*>(t->pcData);
            int w=0,h=0,channels=0;
            stbi_set_flip_vertically_on_load(td.flip_y);
            unsigned char* decoded = stbi_load_from_memory(mem, static_cast<int>(t->mWidth), &w, &h, &channels, 4);
            if (decoded) {
                td.width = w; td.height = h;
                GLuint g = makeGLTextureRGBA8(decoded, w, h, td.srgb, td.generate_mipmaps);
                glTex.push_back(g);
                td.image_bytes.assign(decoded, decoded + static_cast<size_t>(w*h*4));
                stbi_image_free(decoded);
            }
        } else {
            size_t byteCount = static_cast<size_t>(t->mWidth) * static_cast<size_t>(t->mHeight) * sizeof(aiTexel);
            td.image_bytes.resize(byteCount);
            std::memcpy(td.image_bytes.data(), t->pcData, byteCount);
            GLuint tex = 0;
            glCreateTextures(GL_TEXTURE_2D, 1, &tex);
            GLint internalFormat = td.srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
            glTextureStorage2D(tex, 1, internalFormat, td.width, td.height);
            glTextureSubImage2D(tex, 0, 0, 0, td.width, td.height, GL_BGRA, GL_UNSIGNED_BYTE, td.image_bytes.data());
            glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            if (td.generate_mipmaps) glGenerateTextureMipmap(tex);
            glTex.push_back(tex);
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
                stbi_set_flip_vertically_on_load(false);
                unsigned char* data = stbi_load(full.string().c_str(), &w, &h, &ch, 4);
                if (data) {
                    TextureDesc td{};
                    td.image_uuid = p;
                    td.width = w;
                    td.height = h;
                    td.mime_type = "image/unknown";
                    td.wrap_s = 0;
                    td.wrap_t = 0;
                    td.min_filter = 0;
                    td.mag_filter = 0;
                    td.anisotropy = 1;
                    td.srgb = true;
                    td.flip_y = false;
                    td.generate_mipmaps = true;
                    td.image_bytes.assign(data, data + static_cast<size_t>(w*h*4));
                    GLuint g = makeGLTextureRGBA8(data, w, h, td.srgb, td.generate_mipmaps);
                    glTex.push_back(g);
                    stbi_image_free(data);
                    textures_.push_back(std::move(td));
                }
            }
        }
    }
    return glTex;
}
