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
#include "light.h"
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
    Shader depthShader;
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
    if (!depthShader.compileFromFiles("resource/shader/depth.vs", "resource/shader/depth_frag.vs")) {
        std::cerr << "Shader error: " << depthShader.error() << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    Model sceneModel("resource/model/mi.glb");

    UIState uistate;
    Light light;

    int init_w = 0, init_h = 0;
    glfwGetFramebufferSize(window, &init_w, &init_h);
    ui_init(uistate, init_w, init_h);
    light.setupShadowCube(1024, 1.0f, 50.0f);
    // 预创建一个单位立方体，用于后续复用渲染
    // 颜色参数这里给默认值，实际渲染时通过 uniform objectColor 控制
    Cube unitCube(1.0f, 1.0f, 1.0f, glm::vec3(1.0f));

    std::vector<Mesh> extraMeshes;
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int w=0,h=0;
        glfwGetFramebufferSize(window, &w, &h);

        double now = glfwGetTime();
        float dt = float(now - lastTime);
        lastTime = now;
        ui_update_input(uistate, window, dt);
        ui_compute_matrices(uistate, w, h);

        light.setPoint(uistate.light_pos, uistate.light_color);

        //float nearPlane = 1.0f;
        //float farPlane = light.farPlane();
        //glm::vec3 lightPos = light.position();
        //float aspect = 1.0f;
        //glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
        //glm::mat4 shadowTransforms[6];
        //shadowTransforms[0] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        //shadowTransforms[1] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        //shadowTransforms[2] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //shadowTransforms[3] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        //shadowTransforms[4] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        //shadowTransforms[5] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

        //depthShader.use();
        //depthShader.setVec3("lightPos", lightPos);
        //depthShader.setFloat("farPlane", farPlane);

        //light.beginDepthPass();
        //for (int face = 0; face < 6; ++face) {
        //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, light.depthCubeTexture(), 0);
        //    glClear(GL_DEPTH_BUFFER_BIT);

        //    depthShader.setMat4("lightSpaceMatrix", shadowTransforms[face]);
        //    depthShader.setMat4("model", uistate.model);
        //    sceneModel.Draw(depthShader);
        //    for (auto& m : extraMeshes) m.Draw(depthShader);
        //    if (!uistate.cubes.empty()) {
        //        for (const auto& cfg : uistate.cubes) {
        //            if (!cfg.visible) continue;
        //            glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        //            glm::mat4 ry = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        //            glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
        //            glm::mat4 t = glm::translate(glm::mat4(1.0f), cfg.pos);
        //            glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(cfg.scale));
        //            glm::mat4 modelcube = t * rz * ry * rx * s;
        //            depthShader.setMat4("model", modelcube);
        //            Cube c(cfg.length, cfg.width, cfg.height, cfg.color);
        //            c.Draw(depthShader);
        //        }
        //    }
        //}
        //light.endDepthPass();

        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        GLint locModel = shader.uniform("model");
        GLint locView = shader.uniform("view");
        GLint locProj = shader.uniform("projection");
        if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(uistate.model));
        if (locView >= 0) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(uistate.view));
        if (locProj >= 0) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(uistate.projection));

        shader.setVec3("viewPos", uistate.view_pos);
        shader.setVec3("lightPos", light.position());
        shader.setVec3("lightColor", light.color());
        shader.setFloat("outlineWidth", uistate.outlinewidth);
        shader.setInt("texture1", 0);
        //shader.setInt("shadowMap", 1);
        //shader.setFloat("farPlane", farPlane);

        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, light.depthCubeTexture());

        sceneModel.Draw(shader);
        for (auto& m : extraMeshes) m.Draw(shader);

        if (!uistate.cubes.empty()) {
            cubeShader.use();
            GLint cubeModel = cubeShader.uniform("model");
            GLint cubeView = cubeShader.uniform("view");
            GLint cubeProj = cubeShader.uniform("projection");
            if (cubeView >= 0) glUniformMatrix4fv(cubeView, 1, GL_FALSE, glm::value_ptr(uistate.view));
            if (cubeProj >= 0) glUniformMatrix4fv(cubeProj, 1, GL_FALSE, glm::value_ptr(uistate.projection));
            cubeShader.setVec3("lightPos", light.position());
            cubeShader.setVec3("lightColor", light.color());
            //cubeShader.setInt("shadowMap", 1);
            //cubeShader.setFloat("farPlane", farPlane);
            //glActiveTexture(GL_TEXTURE1);
            //glBindTexture(GL_TEXTURE_CUBE_MAP, light.depthCubeTexture());
            for (const auto& cfg : uistate.cubes) {
                if (!cfg.visible) continue;
                glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
                glm::mat4 ry = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
                glm::mat4 t = glm::translate(glm::mat4(1.0f), cfg.pos);
                glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(cfg.scale));
                // 应用长宽高的缩放（因为现在使用单位立方体）
                glm::mat4 sizeScale = glm::scale(glm::mat4(1.0f), glm::vec3(cfg.length, cfg.width, cfg.height));
                glm::mat4 modelcube = t * rz * ry * rx * s * sizeScale;
                
                if (cubeModel >= 0) glUniformMatrix4fv(cubeModel, 1, GL_FALSE, glm::value_ptr(modelcube));
                
                // 设置颜色 Uniform
                cubeShader.setVec3("objectColor", cfg.color);
                
                // 使用复用的单位立方体进行绘制
                unitCube.Draw(cubeShader);
            }
        }

        // 显式解绑 VAO，避免干扰 ImGui
        glBindVertexArray(0);

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
