#include "ui.h"
#include "imgui.h"
#include "GLFW/glfw3.h"

void ui_init(UIState& state, int width, int height) {
    state.ui_active = false;
    state.input_paused = false;
    state.p_last = false;
    state.first_mouse = true;
    state.last_x = width * 0.5;
    state.last_y = height * 0.5;
    state.model = glm::mat4(1.0f);
    state.view_pos = state.camera_pos;
    ui_compute_matrices(state, width, height);
}
void ui_update_input(UIState& state, GLFWwindow* window, float dt) {
    int p = glfwGetKey(window, GLFW_KEY_P);
    if (p == GLFW_PRESS && !state.p_last) {
        state.ui_active = !state.ui_active;
        state.input_paused = state.ui_active;
        glfwSetInputMode(window, GLFW_CURSOR, state.ui_active ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        state.first_mouse = true;
    }
    state.p_last = (p == GLFW_PRESS);
    if (state.input_paused) return;
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
    state.camera_yaw -= dx * state.camera_sensitivity;
    state.camera_pitch -= dy * state.camera_sensitivity;

    if (state.camera_pitch > 360.0f) state.camera_pitch = 360.0f;
    if (state.camera_pitch < -360.0f) state.camera_pitch = -360.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(state.camera_yaw)) * cos(glm::radians(state.camera_pitch));
    front.y = sin(glm::radians(state.camera_pitch));
    front.z = sin(glm::radians(state.camera_yaw)) * cos(glm::radians(state.camera_pitch));
    front = glm::normalize(front);
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) state.camera_pos += front * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) state.camera_pos -= front * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) state.camera_pos -= right * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) state.camera_pos += right * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) state.camera_pos += up * state.camera_speed * dt;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) state.camera_pos -= up * state.camera_speed * dt;
}

void ui_compute_matrices(UIState& state, int width, int height) {
    float aspect = height > 0 ? float(width) / float(height) : 1.777f;
    state.projection = glm::perspective(glm::radians(state.camera_fov), aspect, 0.1f, 200.0f);
    glm::vec3 front;
    front.x = cos(glm::radians(state.camera_yaw)) * cos(glm::radians(state.camera_pitch));
    front.y = sin(glm::radians(state.camera_pitch));
    front.z = sin(glm::radians(state.camera_yaw)) * cos(glm::radians(state.camera_pitch));
    front = glm::normalize(front);
    state.view = glm::lookAt(state.camera_pos, state.camera_pos + front, glm::vec3(0.0f, 1.0f, 0.0f));
    state.view_pos = state.camera_pos;

    glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(state.model_rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 ry = glm::rotate(glm::mat4(1.0f), glm::radians(state.model_rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(state.model_rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 t = glm::translate(glm::mat4(1.0f), state.model_pos);
    glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(state.model_scale));
    state.model = t * rz * ry * rx * s;

    rx = glm::rotate(glm::mat4(1.0f), glm::radians(state.cube_rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    ry = glm::rotate(glm::mat4(1.0f), glm::radians(state.cube_rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rz = glm::rotate(glm::mat4(1.0f), glm::radians(state.cube_rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    t = glm::translate(glm::mat4(1.0f), state.cube_pos);
    s = glm::scale(glm::mat4(1.0f), glm::vec3(state.cube_scale));
    state.modelcube = t * rz * ry * rx * s;
}


void ui_draw(UIState& state) {
    ImGui::Begin("Scene");
    ImGui::Checkbox("UI Active (P)", &state.ui_active);
    ImGui::SliderFloat3("Camera Position", &state.camera_pos.x, -50.0f, 50.0f);
    ImGui::SliderFloat("Yaw", &state.camera_yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &state.camera_pitch, -180.0f, 180.0f);
    ImGui::SliderFloat("Speed", &state.camera_speed, 0.1f, 10.0f);
    ImGui::SliderFloat("FOV", &state.camera_fov, 30.0f, 90.0f);
    ImGui::Separator();
    ImGui::SliderFloat3("Model Pos", &state.model_pos.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Model Rot", &state.model_rot.x, -180.0f, 180.0f);
    ImGui::SliderFloat("Model Scale", &state.model_scale, 0.01f, 10.0f);
    ImGui::Separator();

    ImGui::SliderFloat3("Direcional Light Direction", &state.light_pos.x, -20.0f, 20.0f);
    ImGui::SliderFloat3("Direcional Light Color", &state.light_color.x, 0.0f, 1.0f);
    ImGui::Separator();

    ImGui::SliderFloat3("Cube Pos", &state.cube_pos.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Cube Rot", &state.cube_rot.x, -180.0f, 180.0f);
    ImGui::SliderFloat("Cube Scale", &state.cube_scale, 0.1f, 10.0f);
    ImGui::SliderFloat3("Cube Color", &state.cube_color.x, 0.0f, 1.0f);
    if (ImGui::Button("Create Cube")) state.create_cube = !state.create_cube;
    ImGui::Separator();

    ImGui::DragFloat("outline width", &state.outlinewidth, 0.0001f, 0.0f, 0.1f, "%.4f");
    ImGui::End();
}
