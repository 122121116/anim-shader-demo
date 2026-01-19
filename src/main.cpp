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
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    // 配置 GLFW 上下文版本 (OpenGL 3.3 Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Model Example", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 开启垂直同步

    // 初始化 GLAD (加载 OpenGL 函数指针)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 设置清屏颜色
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    // 初始化 ImGui
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

    // 编译着色器
    Shader shader;      // 主场景着色器
    Shader cubeShader;  // 立方体着色器
    Shader depthShader; // 阴影深度图着色器
    
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

    // 加载模型
    Model sceneModel("resource/model/ark.glb");

    // 初始化 UI 状态和光源
    UIState uistate;
    Light light;

    int init_w = 0, init_h = 0;
    glfwGetFramebufferSize(window, &init_w, &init_h);
    ui_init(uistate, init_w, init_h);
    light.setupShadowCube(2048, 1.0f, 50.0f); // 设置阴影分辨率和裁剪平面
    
    // 预创建一个单位立方体，用于后续复用渲染
    // 颜色参数这里给默认值，实际渲染时通过 uniform objectColor 控制
    Cube unitCube(1.0f, 1.0f, 1.0f, glm::vec3(1.0f));

    std::vector<Mesh> extraMeshes;
    double lastTime = glfwGetTime();

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理窗口事件
        glfwPollEvents();

        int w=0,h=0;
        glfwGetFramebufferSize(window, &w, &h);

        // 计算时间增量
        double now = glfwGetTime();
        float dt = float(now - lastTime);
        lastTime = now;
        
        // 更新 UI 输入和矩阵
        ui_update_input(uistate, window, dt);
        ui_compute_matrices(uistate, w, h);

        light.setPoint(uistate.light_pos, uistate.light_color);

        // ---------------------------------------------------------
        // Pass 1: 阴影贴图生成 (Depth Pass)
        // ---------------------------------------------------------
        float nearPlane = 1.0f;
        float farPlane = light.farPlane();
        glm::vec3 lightPos = light.position();
        float aspect = 1.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
        
        // 生成立方体贴图 6 个面的视图矩阵
        glm::mat4 shadowTransforms[6];
        shadowTransforms[0] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        shadowTransforms[1] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        shadowTransforms[2] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shadowTransforms[3] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        shadowTransforms[4] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        shadowTransforms[5] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

        depthShader.use();
        depthShader.setVec3("lightPos", lightPos);
        depthShader.setFloat("farPlane", farPlane);

        light.beginDepthPass();
        // 渲染场景到深度立方体贴图的 6 个面
        for (int face = 0; face < 6; ++face) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, light.depthCubeTexture(), 0);
            glClear(GL_DEPTH_BUFFER_BIT);

            depthShader.setMat4("lightSpaceMatrix", shadowTransforms[face]);
            depthShader.setMat4("model", uistate.model);
            
            // 绘制主模型
            sceneModel.Draw(depthShader);
            for (auto& m : extraMeshes) m.Draw(depthShader);
            
            // 绘制动态添加的立方体
            if (!uistate.cubes.empty()) {
                for (const auto& cfg : uistate.cubes) {
                    if (!cfg.visible) continue;
                    glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
                    glm::mat4 ry = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
                    glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(cfg.rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
                    glm::mat4 t = glm::translate(glm::mat4(1.0f), cfg.pos);
                    glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(cfg.scale));
                    // Apply scale for unit cube
                    glm::mat4 sizeScale = glm::scale(glm::mat4(1.0f), glm::vec3(cfg.length, cfg.width, cfg.height));
                    glm::mat4 modelcube = t * rz * ry * rx * s * sizeScale;
                    depthShader.setMat4("model", modelcube);
                    
                    // Use unitCube for depth pass too
                    unitCube.Draw(depthShader);
                }
            }
        }
        light.endDepthPass();

        // ---------------------------------------------------------
        // Pass 2: 正常场景渲染 (Lighting Pass)
        // ---------------------------------------------------------
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        // 设置 Uniforms
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
        shader.setInt("shadowMap", 1);
        shader.setFloat("farPlane", farPlane);

        // 绑定阴影贴图
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, light.depthCubeTexture());

        // 绘制主模型
        sceneModel.Draw(shader);
        for (auto& m : extraMeshes) m.Draw(shader);

        // 绘制动态添加的立方体
        if (!uistate.cubes.empty()) {
            cubeShader.use();
            GLint cubeModel = cubeShader.uniform("model");
            GLint cubeView = cubeShader.uniform("view");
            GLint cubeProj = cubeShader.uniform("projection");
            if (cubeView >= 0) glUniformMatrix4fv(cubeView, 1, GL_FALSE, glm::value_ptr(uistate.view));
            if (cubeProj >= 0) glUniformMatrix4fv(cubeProj, 1, GL_FALSE, glm::value_ptr(uistate.projection));
            cubeShader.setVec3("lightPos", light.position());
            cubeShader.setVec3("lightColor", light.color());
            cubeShader.setInt("shadowMap", 1);
            cubeShader.setFloat("farPlane", farPlane);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, light.depthCubeTexture());
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

        // ---------------------------------------------------------
        // UI 渲染
        // ---------------------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 绘制 UI
        ui_draw(uistate);

        // 提交 UI 渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲区
        glfwSwapBuffers(window);
    }
    
    // 清理 ImGui 资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // 清理 GLFW 资源
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
