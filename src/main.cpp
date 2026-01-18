#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader.h"
#include "mesh.h"
#include "model.h"
#include "glm.hpp"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

int main()
{

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Model Example", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    Shader shader;
    if (!shader.compileFromFiles("resource/shader/vertex.vs", "resource/shader/pixel_simple.vs")) {
        std::cerr << "Shader error: " << shader.error() << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }


    Model sceneModel("resource/model/mi.glb");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int w=0,h=0; glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        float aspect = (h>0) ? (float)w/(float)h : 1.777f;
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 200.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 1.5f, 1.0f),
                                     glm::vec3(0.0f, 1.5f, 0.0f),
                                     glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 modelMatrix(1.0f);
        GLint locModel = shader.uniform("model");
        GLint locView = shader.uniform("view");
        GLint locProj = shader.uniform("projection");

        shader.setBool("isOutline", true);
        shader.setFloat("outlineWidth", 0.005f);
        shader.setVec3("lightDir", glm::vec3(0.0f, -1.0f, -1.0f));
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("viewPos", glm::vec3(0.0f, 1.5f, 1.0f));

        if (locModel>=0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        if (locView>=0) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view));
        if (locProj>=0) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));

        shader.use();
        shader.setInt("texture1", 0);

        sceneModel.Draw(shader);

        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


