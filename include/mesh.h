#pragma once
#include <vector>
#include <glad/glad.h>
#include <string>
#include "glm.hpp"
#include "shader.h"

// 顶点结构体，定义了每个顶点的属性
struct Vertex {
    glm::vec3 Position;   // 位置
    glm::vec3 Normal;     // 法线
    glm::vec2 TexCoords;  // 纹理坐标
};

// 纹理结构体
struct Texture {
    unsigned int id;      // OpenGL 纹理 ID
    std::string type;     // 纹理类型 (如 texture_diffuse, texture_specular)
};

// 网格类
// 代表模型中的一个独立网格部分，包含顶点数据、索引数据和材质纹理
class Mesh {
    public:
        /*  网格数据  */
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        /*  函数  */
        // 构造函数：初始化网格数据
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        // 析构函数：释放 OpenGL 资源 (VAO, VBO, EBO)
        ~Mesh();
        
        // 移动构造函数：支持资源所有权转移
        Mesh(Mesh&& other) noexcept;
        // 移动赋值运算符
        Mesh& operator=(Mesh&& other) noexcept;

        // 禁止拷贝构造和赋值，防止 OpenGL 资源重复释放 (RAII)
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        // 绘制网格
        void Draw(Shader &shader);
    private:
        /*  渲染数据  */
        unsigned int VAO, VBO, EBO; // OpenGL 对象 ID
        /*  函数  */
        // 配置网格的 OpenGL 缓冲区和属性指针
        void setupMesh();
};  

