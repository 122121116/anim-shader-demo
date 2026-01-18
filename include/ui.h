#pragma once
#include "thirdparty/glm/glm.hpp"

struct UIState {
    // ===== 1. 摄像机参数 =====
    glm::vec3 camera_pos = glm::vec3(0.0f, 3.5f, 3.0f); 
    float     camera_yaw = -45.0f;
    float     camera_pitch = 20.0f;
    float     camera_speed = 3.0f;
    float     camera_fov = 60.0f;

    // ===== 2. 长方体创建参数=====
    float     cube_width = 1.0f;
    float     cube_height = 1.0f;
    float     cube_depth = 1.0f;
    glm::vec3 cube_pos = glm::vec3(0.0f, 0.0f, 0.0f);

    // ===== 3. 光源位置参数=====
    glm::vec3 light_pos = glm::vec3(2.0f, 3.0f, 5.0f);

    // ===== 4. 模型位置参数 =====
    glm::vec3 model_pos = glm::vec3(0.0f, 0.0f, 0.0f);

    // ===== 辅助标记 =====
    bool load_model = false;
};

void ui_draw(UIState& state);
