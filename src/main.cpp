#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
int test_load_model();
int test_buffer_fill();

int main()
{
    test_buffer_fill();
    test_load_model();
    // ========== 1. ��ʼ�� GLFW ==========
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    // ���� OpenGL �汾�������ļ�
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //���Լ���
    

    // ========== 2. ���� GLFW ���� ==========
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui + GLFW + OpenGL3", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // ========== 3. ���õ�ǰ������ ==========
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // ������ֱͬ��

    // ========== 4. ��ʼ�� GLAD�������ڴ������ں� ==========
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // ========== 5. ��ʼ�� ImGui ������ ==========
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // ���� ImGui ��ʽ����ѡ��
    ImGui::StyleColorsDark();

    // ���ü���/������
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // ========== 6. 初始化 ImGui 平台后端（GLFW） ==========
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        std::cerr << "Failed to initialize ImGui GLFW backend" << std::endl;
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // ========== 7. 初始化 ImGui 渲染后端（OpenGL3） ==========
    const char* glsl_version = "#version 330";
    if (!ImGui_ImplOpenGL3_Init(glsl_version))
    {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend" << std::endl;
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // ========== 8. 设置清除颜色 ==========
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);

    // ========== 主循环 ==========
    while (!glfwWindowShouldClose(window))
    {
        // 处理 GLFW 事件
        glfwPollEvents();

        // 开始新帧（ImGui）
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ========== 绘制 ImGui 界面 ==========
        {
            // 1. 显示一个简单的窗口
            ImGui::Begin("Hello, ImGui!");
            ImGui::Text("This is a simple ImGui window.");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / io.Framerate, io.Framerate);

            static float f = 0.0f;
            ImGui::SliderFloat("Float", &f, 0.0f, 1.0f);

            static int counter = 0;
            if (ImGui::Button("Click Me!"))
                counter++;
            ImGui::SameLine();
            ImGui::Text("Counter = %d", counter);

            ImGui::End();

            // 2. 显示另一个窗口示例
            ImGui::Begin("Another Window");
            ImGui::Text("You can create multiple windows.");
            ImGui::End();
        }

        // ========== 渲染 ==========
        // 清除缓冲区
        glClear(GL_COLOR_BUFFER_BIT);

        // 渲染 ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲区
        glfwSwapBuffers(window);
    }

    // ========== 清理资源（按创建的反序） ==========
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


