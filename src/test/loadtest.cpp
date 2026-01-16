#include "load.h"
#include <iostream>
#include <filesystem>
int test_load_model() {
    std::filesystem::path modelDir = "resource/model";
#ifdef PROJECT_MODEL_DIR
    modelDir = std::filesystem::path(PROJECT_MODEL_DIR);
#endif

    Load loader;
    if (!loader.loadFromFile("resource/model/mi.glb")) {
        std::cerr << "Failed to load glb: " << (loader.getError() ? loader.getError() : "") << "\n";
        return -1;
    }
    std::cout << "Meshes: " << loader.getMeshCount() << "\n";
    std::cout << "Materials: " << loader.getMaterialCount() << "\n";
    std::cout << "EmbeddedTextures: " << loader.getEmbeddedTextureCount() << "\n";
    std::cout << "Lights: " << loader.getLightCount() << "\n";
    std::cout << "Cameras: " << loader.getCameraCount() << "\n";
    std::cout << "Animations: " << loader.getAnimationCount() << "\n";

    const aiMesh* m = loader.getMesh(0);
    const aiMaterial* mat = loader.getMaterial(0);
    const aiTexture* tex = loader.getEmbeddedTexture(0);
    const aiLight* light = loader.getLight(0);
    const aiCamera* cam = loader.getCamera(0);
    const aiAnimation* anim = loader.getAnimation(0);

    return 0;
}
