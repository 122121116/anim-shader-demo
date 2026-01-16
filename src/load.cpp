#include "load.h"
#include <stdexcept>
Load::Load() : scene_(nullptr) {}
bool Load::loadFromFile(const std::string& path, unsigned int flags) {
    path_ = path;
    scene_ = importer_.ReadFile(path, flags);
    return scene_ != nullptr;
}
const aiScene* Load::getScene() const {
    return scene_;
}
const aiNode* Load::getRootNode() const {
    if (!scene_) return nullptr;
    return scene_->mRootNode;
}
size_t Load::getMeshCount() const {
    if (!scene_) return 0;
    return static_cast<size_t>(scene_->mNumMeshes);
}
const aiMesh* Load::getMesh(size_t index) const {
    if (!scene_) return nullptr;
    if (index >= scene_->mNumMeshes) return nullptr;
    return scene_->mMeshes[index];
}
size_t Load::getMaterialCount() const {
    if (!scene_) return 0;
    return static_cast<size_t>(scene_->mNumMaterials);
}
const aiMaterial* Load::getMaterial(size_t index) const {
    if (!scene_) return nullptr;
    if (index >= scene_->mNumMaterials) return nullptr;
    return scene_->mMaterials[index];
}
size_t Load::getEmbeddedTextureCount() const {
    if (!scene_) return 0;
    return static_cast<size_t>(scene_->mNumTextures);
}
const aiTexture* Load::getEmbeddedTexture(size_t index) const {
    if (!scene_) return nullptr;
    if (index >= scene_->mNumTextures) return nullptr;
    return scene_->mTextures[index];
}
size_t Load::getLightCount() const {
    if (!scene_) return 0;
    return static_cast<size_t>(scene_->mNumLights);
}
const aiLight* Load::getLight(size_t index) const {
    if (!scene_) return nullptr;
    if (index >= scene_->mNumLights) return nullptr;
    return scene_->mLights[index];
}
size_t Load::getCameraCount() const {
    if (!scene_) return 0;
    return static_cast<size_t>(scene_->mNumCameras);
}
const aiCamera* Load::getCamera(size_t index) const {
    if (!scene_) return nullptr;
    if (index >= scene_->mNumCameras) return nullptr;
    return scene_->mCameras[index];
}
size_t Load::getAnimationCount() const {
    if (!scene_) return 0;
    return static_cast<size_t>(scene_->mNumAnimations);
}
const aiAnimation* Load::getAnimation(size_t index) const {
    if (!scene_) return nullptr;
    if (index >= scene_->mNumAnimations) return nullptr;
    return scene_->mAnimations[index];
}
const std::string& Load::getPath() const {
    return path_;
}
const char* Load::getError() const {
    return importer_.GetErrorString();
}
unsigned int Load::defaultFlags() {
    return aiProcess_Triangulate |
           aiProcess_GenSmoothNormals |
           aiProcess_CalcTangentSpace |
           aiProcess_JoinIdenticalVertices |
           aiProcess_ImproveCacheLocality |
           aiProcess_SortByPType |
           aiProcess_RemoveRedundantMaterials |
           aiProcess_FindInvalidData |
           aiProcess_ValidateDataStructure;
}
