#include "mesh.h"
#include <iostream>

// 构造函数：初始化网格数据并配置 OpenGL 资源
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

// 析构函数：释放 OpenGL 缓冲区
Mesh::~Mesh() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

// 移动构造函数：接管另一个 Mesh 的资源
Mesh::Mesh(Mesh&& other) noexcept 
    : vertices(std::move(other.vertices))
    , indices(std::move(other.indices))
    , textures(std::move(other.textures))
    , VAO(other.VAO)
    , VBO(other.VBO)
    , EBO(other.EBO)
{
    // 将原对象的 ID 置零，防止析构时误删
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}

// 移动赋值运算符
Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        // 先释放当前对象的资源
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);

        // 接管资源
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        textures = std::move(other.textures);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;

        // 置空原对象
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

// 配置 OpenGL 缓冲区 (VAO, VBO, EBO)
void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 上传顶点数据
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // 上传索引数据
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_STATIC_DRAW);

    // 顶点位置 (Location 0)
    glEnableVertexAttribArray(0);   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 顶点法线 (Location 1)
    glEnableVertexAttribArray(1);   
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // 顶点纹理坐标 (Location 2)
    glEnableVertexAttribArray(2);   
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}  

// 绘制网格
void Mesh::Draw(Shader &shader) 
{   
    bool hasTex = false;
    // 如果有纹理，绑定第一张纹理
    if (!textures.empty()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0].id);
        hasTex = true;
    } 

    // 设置 Shader 中的 hasTexture 和 objectColor
    // 如果没有纹理，使用白色作为基础色
    shader.setInt("hasTexture", hasTex ? 1 : 0);
    if (!hasTex) {
        shader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // 绘制调用
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
