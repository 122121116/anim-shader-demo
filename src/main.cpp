#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader.h"
#include "mesh.h"
#include "model.h"
#include "glm.hpp"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "ui.h"
#include "cube.h"
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "ImGui_ImplGlfw_InitForOpenGL failed" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        std::cerr << "ImGui_ImplOpenGL3_Init failed" << std::endl;
        ImGui_ImplGlfw_Shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    Shader shader;
    Shader cubeShader;
    if (!shader.compileFromFiles("resource/shader/vertex.vs", "resource/shader/pixel.vs")) {
        std::cerr << "Shader error: " << shader.error() << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    if (!cubeShader.compileFromFiles("resource/shader/vertex_cube.vs", "resource/shader/pixel_cube.vs")) {
        std::cerr << "Shader error: " << cubeShader.error() << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    Model sceneModel("resource/model/mi.glb");

    UIState uistate;

    int init_w = 0, init_h = 0;
    glfwGetFramebufferSize(window, &init_w, &init_h);
    ui_init(uistate, init_w, init_h);
    std::vector<Mesh> extraMeshes;
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int w=0,h=0; glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        double now = glfwGetTime();
        float dt = float(now - lastTime);
        lastTime = now;
        ui_update_input(uistate, window, dt);
        ui_compute_matrices(uistate, w, h);

        shader.use();
        GLint locModel = shader.uniform("model");
        GLint locView = shader.uniform("view");
        GLint locProj = shader.uniform("projection");
        if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(uistate.model));
        if (locView >= 0) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(uistate.view));
        if (locProj >= 0) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(uistate.projection));

        shader.setVec3("viewPos", uistate.view_pos);
        shader.setVec3("lightDir", uistate.light_pos);
        shader.setVec3("lightColor", uistate.light_color);
        shader.setFloat("outlineWidth", uistate.outlinewidth);
        shader.setInt("texture1", 0);

        sceneModel.Draw(shader);
        for (auto& m : extraMeshes) m.Draw(shader);

        if (!uistate.cubes.empty()) {
            cubeShader.use();
            GLint cubeModel = cubeShader.uniform("model");
            GLint cubeView = cubeShader.uniform("view");
            GLint cubeProj = cubeShader.uniform("projection");
            if (cubeView >= 0) glUniformMatrix4fv(cubeView, 1, GL_FALSE, glm::value_ptr(uistate.view));
            if (cubeProj >= 0) glUniformMatrix4fv(cubeProj, 1, GL_FALSE, glm::value_ptr(uistate.projection));
            cubeShader.setVec3("lightDir", uistate.light_pos);
            cubeShader.setVec3("lightColor", uistate.light_color);
            for (const auto& cfg : uistate.cubes) {
                if (!cfg.visible) continue;
                glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
                glm::mat4 ry = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
                glm::mat4 t = glm::translate(glm::mat4(1.0f), cfg.pos);
                glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(cfg.scale));
                glm::mat4 modelcube = t * rz * ry * rx * s;
                if (cubeModel >= 0) glUniformMatrix4fv(cubeModel, 1, GL_FALSE, glm::value_ptr(modelcube));
                Cube c(cfg.length, cfg.width, cfg.height, cfg.color);
                c.Draw(cubeShader);
            }
        }

        // 开始 UI 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 绘制 UI
        ui_draw(uistate);

        // 提交 UI 渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


