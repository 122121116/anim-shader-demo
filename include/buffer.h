#pragma once
#include <vector>
#include <glad/glad.h>
#include "attribute.h"

class MeshBuffer {
public:
    MeshBuffer();
    ~MeshBuffer();
    void setData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void bind() const;
    void unbind() const;
    void draw() const;
    GLuint vao() const;
    GLuint vbo() const;
    GLuint ebo() const;
private:
    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;
    GLsizei indexCount_;
};
