#pragma once
#include <string>
#include <vector>
#include "attribute.h"
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class Load {
public:
    Load();
    bool loadFromFile(const std::string& path);
    const std::vector<Vertex>& vertices() const;
    const std::vector<MaterialDesc>& materials() const;
    const std::vector<TextureDesc>& textures() const;
    const std::vector<GLuint>& texturesGL() const;
    const std::string& getPath() const;
    const char* getError() const;
private:
    void clear();
    void extractVertices(const aiScene* scene);
    void extractMaterials(const aiScene* scene);
    std::vector<GLuint> extractTextures(const aiScene* scene, const std::string& baseDir);
    std::string path_;
    std::vector<Vertex> vertices_;
    std::vector<MaterialDesc> materials_;
    std::vector<TextureDesc> textures_;
    std::vector<GLuint> textures_gl_;
    Assimp::Importer importer_;
    const aiScene* scene_;
    std::string error_;
};
