#include "cube.h"

Cube::Cube(float length, float width, float height, glm::vec3 color) {
    build(length, width, height, color);
}

void Cube::build(float length, float width, float height, glm::vec3 color) {
    // 定义立方体的8个顶点
    vertices_ = {
        {{-length / 2, -width / 2, -height / 2}, {0.0f, 0.0f, -1.0f}, color},
        {{-length / 2, -width / 2, height / 2}, {0.0f, 0.0f, 1.0f}, color},
        {{-length / 2, width / 2, height / 2}, {0.0f, 1.0f, 1.0f}, color},
        {{-length / 2, width / 2, -height / 2}, {0.0f, 1.0f, -1.0f}, color},
        {{length / 2, -width / 2, -height / 2}, {1.0f, 0.0f, -1.0f}, color},
        {{length / 2, -width / 2, height / 2}, {1.0f, 0.0f, 1.0f}, color},
        {{length / 2, width / 2, height / 2}, {1.0f, 1.0f, 1.0f}, color},
        {{length / 2, width / 2, -height / 2}, {1.0f, 1.0f, -1.0f}, color}
    };
    // 定义立方体的6个面（每个面2个三角形）
    indices_ = {
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        7, 6, 5, 5, 4, 7,
        4, 0, 3, 3, 7, 4,
        3, 2, 6, 6, 7, 3,
        4, 5, 1, 1, 0, 4
    };
    // 生成VAO、VBO、EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // 绑定VAO
    glBindVertexArray(VAO);
    // 绑定VBO并上传顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(VertexCube), vertices_.data(), GL_STATIC_DRAW);
    // 绑定EBO并上传索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);
    // 设置顶点属性指针
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCube), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCube), (void*)offsetof(VertexCube, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCube), (void*)offsetof(VertexCube, Color));
    glBindVertexArray(0);
}

void Cube::Draw(Shader &shader) {
    // 绑定VAO
    glBindVertexArray(VAO);
    // 绘制立方体
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
    // 解绑VAO
    glBindVertexArray(0);
}
