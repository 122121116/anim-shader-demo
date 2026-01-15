#pragma once
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class Load {
public:
    Load();
    bool loadFromFile(const std::string& path, unsigned int flags = Load::defaultFlags());
    const aiScene* getScene() const;
    const aiNode* getRootNode() const;
    size_t getMeshCount() const;
    const aiMesh* getMesh(size_t index) const;
    size_t getMaterialCount() const;
    const aiMaterial* getMaterial(size_t index) const;
    size_t getEmbeddedTextureCount() const;
    const aiTexture* getEmbeddedTexture(size_t index) const;
    size_t getLightCount() const;
    const aiLight* getLight(size_t index) const;
    size_t getCameraCount() const;
    const aiCamera* getCamera(size_t index) const;
    size_t getAnimationCount() const;
    const aiAnimation* getAnimation(size_t index) const;
    const std::string& getPath() const;
    static unsigned int defaultFlags();
private:
    std::string path_;
    const aiScene* scene_;
    Assimp::Importer importer_;
};
