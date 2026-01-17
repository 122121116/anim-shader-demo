#include <iostream>
#include "cube.h"

int test_cube() {
    Cube cube(1.0f);

    std::cout << "[Cube Test]\n";
    std::cout << "Vertices = " << cube.vertices().size() << "\n";
    std::cout << "Indices = " << cube.indices().size() << "\n";

    if (cube.vertices().size() == 24 && cube.indices().size() == 36) {
        std::cout << "Cube geometry OK\n";
        return 0;
    }

    std::cout << "Cube geometry ERROR\n";
    return -1;
}
