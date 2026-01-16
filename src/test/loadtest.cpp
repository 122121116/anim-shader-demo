#include "load.h"
#include <iostream>

int test_load_model() {

    Load loader;
    if (!loader.loadFromFile("resource/model/mi.glb")) {
        std::cerr << "Failed to load glb: " << (loader.getError() ? loader.getError() : "") << "\n";
        return -1;
    }
    std::cout << "Vertices: " << loader.vertices().size() << "\n";
    std::cout << "Materials: " << loader.materials().size() << "\n";
    std::cout << "Textures: " << loader.textures().size() << "\n";

    return 0;
}
