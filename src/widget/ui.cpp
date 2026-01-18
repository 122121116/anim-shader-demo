#include "ui.h"
#include "imgui.h"

void ui_draw(UIState& state) {
    ImGui::Begin("Scene & Param Control");

    // ========== 摄像机参数调节 ==========
    ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::SliderFloat3("Camera Position", &state.camera_pos.x, -10.0f, 10.0f);
    ImGui::SliderFloat("Yaw", &state.camera_yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &state.camera_pitch, -89.0f, 89.0f);
    ImGui::SliderFloat("Move Speed", &state.camera_speed, 0.1f, 10.0f);
    ImGui::SliderFloat("FOV", &state.camera_fov, 30.0f, 90.0f);
    ImGui::Separator();

    // ========== 长方体创建 ==========
    ImGui::CollapsingHeader("Cube Create (Occlusion Test)", ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::Text("Cube Size");
    ImGui::SliderFloat("Width", &state.cube_width, 0.2f, 3.0f);
    ImGui::SliderFloat("Height", &state.cube_height, 0.2f, 3.0f);
    ImGui::SliderFloat("Depth", &state.cube_depth, 0.2f, 3.0f);
    ImGui::SliderFloat3("Cube Position", &state.cube_pos.x, -5.0f, 5.0f);
    if (ImGui::Button("Create Cube")) {
        state.load_model = true;
    }
    ImGui::Separator();

    // ========== 光源位置调节 ==========
    ImGui::CollapsingHeader("Light Settings", ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::SliderFloat3("Light Position", &state.light_pos.x, -10.0f, 10.0f);
    ImGui::Separator();

    // ========== 模型位置调 ==========
    ImGui::CollapsingHeader("Model Settings", ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::SliderFloat3("Model Position", &state.model_pos.x, -5.0f, 5.0f);

    ImGui::End();
}
