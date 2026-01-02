#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
struct MeshData {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<unsigned int> indices;
};
class Loader {
public:
    MeshData loadModel(const std::string& path);
    GLuint loadTexture(const std::string& path, bool flip = true);
};
