#include "ui.h"
#include "imgui.h"
#include "GLFW/glfw3.h"

// 初始化 UI 状态
// 设置默认参数、相机位置、立方体属性等
void ui_init(UIState& state, int width, int height) {
    state.ui_active = false;      // 初始状态下禁用 UI 交互（鼠标控制相机）
    state.input_paused = false;
    state.p_last = false;
    state.first_mouse = true;
    state.last_x = width * 0.5;   // 鼠标初始位置设为屏幕中心
    state.last_y = height * 0.5;
    state.model = glm::mat4(1.0f);
    state.view_pos = state.camera_pos;
    state.cubes.clear();
    
    // 初始化第一个立方体的配置
    CubeConfig c;
    c.length = state.cube_length;
    c.width = state.cube_width;
    c.height = state.cube_height;
    c.pos = state.cube_pos;
    c.scale = state.cube_scale;
    c.color = state.cube_color;
    c.rot = state.cube_rot;
    c.visible = true;
    state.cubes.push_back(c);
    state.selected_cube = 0;
    
    // 计算初始矩阵
    ui_compute_matrices(state, width, height);
}

// 处理输入更新
// 响应键盘按键（移动相机、切换 UI 模式）和鼠标移动（旋转视角）
void ui_update_input(UIState& state, GLFWwindow* window, float dt) {
    // 切换 UI 激活状态 (按 P 键)
    int p = glfwGetKey(window, GLFW_KEY_P);
    if (p == GLFW_PRESS && !state.p_last) {
        state.ui_active = !state.ui_active;
        state.input_paused = state.ui_active;
        // 激活 UI 时显示鼠标光标，否则隐藏并锁定
        glfwSetInputMode(window, GLFW_CURSOR, state.ui_active ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        state.first_mouse = true;
    }
    state.p_last = (p == GLFW_PRESS);
    
    // 如果 UI 处于激活状态，暂停相机控制
    if (state.input_paused) return;
    
    // 获取鼠标位置
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (state.first_mouse) {
        state.last_x = x;
        state.last_y = y;
        state.first_mouse = false;
    }
    float dx = float(x - state.last_x);
    float dy = float(state.last_y - y);
    state.last_x = x;
    state.last_y = y;
    
    // 更新相机偏航角和俯仰角
    state.camera_yaw += dx * state.camera_sensitivity;
    state.camera_pitch += dy * state.camera_sensitivity;

    // 限制俯仰角范围，防止万向节死锁或视角翻转
    if (state.camera_pitch > 360.0f) state.camera_pitch = 360.0f;
    if (state.camera_pitch < -360.0f) state.camera_pitch = -360.0f;

    // 计算相机前向量
    glm::vec3 front;
    front.x = cos(glm::radians(state.camera_yaw)) * cos(glm::radians(state.camera_pitch));
    front.y = sin(glm::radians(state.camera_pitch));
    front.z = sin(glm::radians(state.camera_yaw)) * cos(glm::radians(state.camera_pitch));
    front = glm::normalize(front);
    
    // 计算右向量和上向量
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    // 键盘控制相机移动 (WASD + Space/Ctrl)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) state.camera_pos += front * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) state.camera_pos -= front * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) state.camera_pos -= right * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) state.camera_pos += right * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) state.camera_pos += up * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) state.camera_pos -= up * state.camera_speed * dt;
}

// 计算变换矩阵
// 更新投影矩阵、视图矩阵和模型矩阵
void ui_compute_matrices(UIState& state, int width, int height) {
    // 投影矩阵
    float aspect = height > 0 ? float(width) / float(height) : 1.777f;
    state.projection = glm::perspective(glm::radians(state.camera_fov), aspect, 0.1f, 200.0f);
    
    // 视图矩阵
    glm::vec3 front;
    front.x = cos(glm::radians(state.camera_yaw)) * cos(glm::radians(state.camera_pitch));
    front.y = sin(glm::radians(state.camera_pitch));
    front.z = sin(glm::radians(state.camera_yaw)) * cos(glm::radians(state.camera_pitch));
    front = glm::normalize(front);
    state.view = glm::lookAt(state.camera_pos, state.camera_pos + front, glm::vec3(0.0f, 1.0f, 0.0f));
    state.view_pos = state.camera_pos;

    // 模型矩阵（注意变换顺序：缩放 -> 旋转 -> 平移）
    glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(state.model_rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 ry = glm::rotate(glm::mat4(1.0f), glm::radians(state.model_rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(state.model_rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 t = glm::translate(glm::mat4(1.0f), state.model_pos);
    glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(state.model_scale));
    state.model = t * rz * ry * rx * s;
}


// 绘制 UI 界面
// 使用 ImGui 绘制参数调节面板
void ui_draw(UIState& state) {
    ImGui::Begin("Scene");
    
    // 相机和视图控制
    ImGui::Checkbox("UI Active (P)", &state.ui_active);
    ImGui::SliderFloat3("Camera Position", &state.camera_pos.x, -50.0f, 50.0f);
    ImGui::SliderFloat("Yaw", &state.camera_yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &state.camera_pitch, -180.0f, 180.0f);
    ImGui::SliderFloat("Speed", &state.camera_speed, 0.1f, 10.0f);
    ImGui::SliderFloat("FOV", &state.camera_fov, 30.0f, 90.0f);
    ImGui::Separator();
    
    // 模型变换控制
    ImGui::SliderFloat3("Model Pos", &state.model_pos.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Model Rot", &state.model_rot.x, -180.0f, 180.0f);
    ImGui::SliderFloat("Model Scale", &state.model_scale, 0.01f, 10.0f);
    ImGui::Separator();

    // 光源控制
    ImGui::SliderFloat3("Light Pos", &state.light_pos.x, -20.0f, 20.0f);
    ImGui::SliderFloat3("Light Color", &state.light_color.x, 0.0f, 1.0f);
    ImGui::Separator();

    // 立方体管理
    ImGui::Text("Cubes");
    if (ImGui::Button("Add Cube")) {
        CubeConfig c;
        c.length = state.cube_length;
        c.width = state.cube_width;
        c.height = state.cube_height;
        c.pos = state.cube_pos;
        c.scale = state.cube_scale;
        c.color = state.cube_color;
        c.rot = state.cube_rot;
        c.visible = true;
        state.cubes.push_back(c);
        state.selected_cube = int(state.cubes.size()) - 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove Selected")) {
        if (state.selected_cube >= 0 && state.selected_cube < int(state.cubes.size())) {
            state.cubes.erase(state.cubes.begin() + state.selected_cube);
            if (state.cubes.empty()) {
                state.selected_cube = -1;
            } else if (state.selected_cube >= int(state.cubes.size())) {
                state.selected_cube = int(state.cubes.size()) - 1;
            }
        }
    }
    
    // 当前选中立方体的属性编辑
    int count = int(state.cubes.size());
    if (count > 0) {
        if (state.selected_cube < 0 || state.selected_cube >= count) state.selected_cube = 0;
        ImGui::SliderInt("Selected Cube", &state.selected_cube, 0, count - 1);
        CubeConfig& c = state.cubes[state.selected_cube];
        ImGui::Checkbox("Visible", &c.visible);
        ImGui::SliderFloat3("Cube Pos", &c.pos.x, -10.0f, 10.0f);
        ImGui::SliderFloat3("Cube Rot", &c.rot.x, -180.0f, 180.0f);
        ImGui::SliderFloat("Cube Scale", &c.scale, 0.1f, 10.0f);
        ImGui::SliderFloat3("Cube Color", &c.color.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Cube Length", &c.length, 0.1f, 10.0f);
        ImGui::SliderFloat("Cube Width", &c.width, 0.1f, 10.0f);
        ImGui::SliderFloat("Cube Height", &c.height, 0.1f, 10.0f);
    }
    ImGui::Separator();

    // 其他设置
    ImGui::DragFloat("outline width", &state.outlinewidth, 0.0001f, 0.0f, 0.1f, "%.4f");
    ImGui::End();
}
