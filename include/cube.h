#pragma once
#include <vector>
#include <glad/glad.h>
#include <string>
#include "glm.hpp"
#include "shader.h"

// 立方体顶点结构
struct VertexCube {
    glm::vec3 Position; // 位置
    glm::vec3 Normal;   // 法线
    glm::vec3 Color;    // 颜色
};

// 立方体类
// 用于程序化生成和渲染立方体几何体
class Cube {
public:
    // 构造函数
    // length, width, height: 立方体的长宽高
    // color: 立方体的基础颜色
    Cube(float length, float width, float height, glm::vec3 color);
    ~Cube();

    // 禁止拷贝构造和赋值操作，防止 VAO/VBO 双重释放 (RAII)
    Cube(const Cube&) = delete;
    Cube& operator=(const Cube&) = delete;

    // 顶点数据和索引数据
    std::vector<VertexCube> vertices_;
    std::vector<unsigned int> indices_;

    // 获取顶点数据
    const std::vector<VertexCube>& vertices() const { return vertices_; }
    // 获取索引数据
    const std::vector<unsigned int>& indices() const { return indices_; }
    
    // 绘制立方体
    void Draw(Shader &shader);

private:
    unsigned int VAO, VBO, EBO; // OpenGL 资源 ID

    // 构建立方体网格数据
    void build(float length, float width, float height, glm::vec3 color);
};
