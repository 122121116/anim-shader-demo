#include "ui.h"
#include "imgui.h"

void ui_draw(UIState& state) {
    ImGui::Begin("Scene Control");

    ImGui::Text("Camera Settings");
    ImGui::SliderFloat("Speed", &state.camera_speed, 0.1f, 10.0f);
    ImGui::SliderFloat("FOV", &state.camera_fov, 30.0f, 90.0f);

    ImGui::Separator();

    if (ImGui::Button("Load Model")) {
        state.load_model = true;
    }

    ImGui::End();
}
