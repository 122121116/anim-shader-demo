#include "buffer.h"
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <cstring>

int test_buffer_fill() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* win = glfwCreateWindow(640, 480, "bufferfill", nullptr, nullptr);
    if (!win) {
        std::cerr << "GLFW window failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    if (gladLoadGL() == 0) {
        std::cerr << "GL load failed\n";
        glfwDestroyWindow(win);
        glfwTerminate();
        return -1;
    }

    std::vector<Vertex> verts(3);
    std::memset(verts.data(), 0, verts.size() * sizeof(Vertex));
    verts[0].position[0] = -0.5f; verts[0].position[1] = -0.5f; verts[0].position[2] = 0.0f;
    verts[1].position[0] =  0.5f; verts[1].position[1] = -0.5f; verts[1].position[2] = 0.0f;
    verts[2].position[0] =  0.0f; verts[2].position[1] =  0.5f; verts[2].position[2] = 0.0f;
    std::vector<unsigned int> indices = {0,1,2};

    MeshBuffer mb;
    mb.setData(verts, indices);
    mb.bind();

    GLint vboSize = 0;
    glBindBuffer(GL_ARRAY_BUFFER, mb.vbo());
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vboSize);

    GLint eboSize = 0;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mb.ebo());
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &eboSize);

    std::cout << "Expected VBO bytes: " << (verts.size() * sizeof(Vertex)) << "\n";
    std::cout << "Actual   VBO bytes: " << vboSize << "\n";
    std::cout << "Expected EBO bytes: " << (indices.size() * sizeof(unsigned int)) << "\n";
    std::cout << "Actual   EBO bytes: " << eboSize << "\n";

    mb.unbind();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
