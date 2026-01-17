
#pragma once

struct UIState {
    float camera_speed = 3.0f;
    float camera_fov = 60.0f;
    bool load_model = false;

    bool mouse_left_down = false; 
    float camera_yaw = -90.0f;  
    float camera_pitch = 0.0f;
};

void ui_draw(UIState& state);
