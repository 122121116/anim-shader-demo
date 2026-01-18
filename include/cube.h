#pragma once
#include <vector>
#include <glad/glad.h>
#include <string>
#include "glm.hpp"
#include "shader.h"

struct VertexCube {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Color;
};

class Cube {
public:
    Cube(float length, float width, float height, glm::vec3 color);
    ~Cube();

    // 禁止拷贝构造和赋值操作，防止 VAO/VBO 双重释放
    Cube(const Cube&) = delete;
    Cube& operator=(const Cube&) = delete;

    std::vector<VertexCube> vertices_;
    std::vector<unsigned int> indices_;

    const std::vector<VertexCube>& vertices() const { return vertices_; }
    const std::vector<unsigned int>& indices() const { return indices_; }
    void Draw(Shader &shader);

private:
    unsigned int VAO, VBO, EBO;

    void build(float length, float width, float height, glm::vec3 color);
};
