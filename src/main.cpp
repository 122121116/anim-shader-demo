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
#include "shadow.h"

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
    Shader depthShader;
    PointLightShadow pointShadow;
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

    bool shadowShaderOk = depthShader.compileFromFiles("resource/shader/depth_point.vs", "resource/shader/depth_point.fs");
    bool shadowOk = false;
    if (shadowShaderOk) {
        shadowOk = pointShadow.init(1024, 0.1f, 50.0f);
        if (!shadowOk) {
            std::cerr << "Shadow framebuffer init failed" << std::endl;
        }
    } else {
        std::cerr << "Shadow shader error: " << depthShader.error() << std::endl;
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

        if (shadowOk) {
            pointShadow.setLightPos(uistate.light_pos);
            std::vector<glm::mat4> shadowMatrices;
            pointShadow.computeMatrices(shadowMatrices);

            glViewport(0, 0, pointShadow.size(), pointShadow.size());
            glBindFramebuffer(GL_FRAMEBUFFER, pointShadow.fbo());
            glClear(GL_DEPTH_BUFFER_BIT);

            depthShader.use();
            GLint depthModelLoc = depthShader.uniform("model");
            GLint depthMatrixLoc = depthShader.uniform("shadowMatrix");
            GLint depthLightPosLoc = depthShader.uniform("lightPos");
            GLint depthFarPlaneLoc = depthShader.uniform("far_plane");

            if (depthLightPosLoc >= 0) glUniform3fv(depthLightPosLoc, 1, glm::value_ptr(uistate.light_pos));
            if (depthFarPlaneLoc >= 0) glUniform1f(depthFarPlaneLoc, pointShadow.farPlane());

            for (int face = 0; face < 6; ++face) {
                if (depthMatrixLoc >= 0) glUniformMatrix4fv(depthMatrixLoc, 1, GL_FALSE, glm::value_ptr(shadowMatrices[face]));
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, pointShadow.depthMap(), 0);
                glClear(GL_DEPTH_BUFFER_BIT);

                if (depthModelLoc >= 0) glUniformMatrix4fv(depthModelLoc, 1, GL_FALSE, glm::value_ptr(uistate.model));
                sceneModel.Draw(depthShader);
                for (auto& m : extraMeshes) {
                    if (depthModelLoc >= 0) glUniformMatrix4fv(depthModelLoc, 1, GL_FALSE, glm::value_ptr(uistate.model));
                    m.Draw(depthShader);
                }

                if (!uistate.cubes.empty()) {
                    for (const auto& cfg : uistate.cubes) {
                        if (!cfg.visible) continue;
                        glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
                        glm::mat4 ry = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
                        glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
                        glm::mat4 t = glm::translate(glm::mat4(1.0f), cfg.pos);
                        glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(cfg.scale));
                        glm::mat4 modelcube = t * rz * ry * rx * s;
                        if (depthModelLoc >= 0) glUniformMatrix4fv(depthModelLoc, 1, GL_FALSE, glm::value_ptr(modelcube));
                        Cube c(cfg.length, cfg.width, cfg.height, cfg.color);
                        c.Draw(depthShader);
                    }
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, w, h);
        }

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
        shader.setBool("useShadow", shadowOk);
        if (shadowOk) {
            shader.setVec3("lightPos", uistate.point_light_pos);
            shader.setVec3("pointLightColor", uistate.point_light_color);
            shader.setFloat("far_plane", pointShadow.farPlane());
            shader.setInt("shadowMap", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadow.depthMap());
        }

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
            cubeShader.setBool("useShadow", shadowOk);
            if (shadowOk) {
                cubeShader.setVec3("lightPos", uistate.point_light_pos);
                cubeShader.setVec3("pointLightColor", uistate.point_light_color);
                cubeShader.setFloat("far_plane", pointShadow.farPlane());
                cubeShader.setInt("shadowMap", 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadow.depthMap());
            }
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


