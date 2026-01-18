#pragma once
#include "cube.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
struct GLFWwindow;
struct UIState {
    bool ui_active = false;
    bool input_paused = false;
    bool p_last = false;

    float outlinewidth = 0.0001f;

    glm::vec3 camera_pos = glm::vec3(0.0f, 1.5f, 1.0f);
    float camera_yaw = -90.0f;
    float camera_pitch = 0.0f;
    float camera_speed = 2.5f;
    float camera_sensitivity = 0.1f;
    float camera_fov = 60.0f;

    glm::vec3 model_rot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 model_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    float model_scale = 1.0f;

    glm::vec3 light_pos = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 view_pos;

    glm::mat4 modelcube;

    bool create_cube = false;
    float cube_length = 1.0f;
    float cube_width = 1.0f;
    float cube_height = 1.0f;
    glm::vec3 cube_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    float cube_scale = 1.0f;
    glm::vec3 cube_color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 cube_rot = glm::vec3(0.0f, 0.0f, 0.0f);


    double last_x = 0.0;
    double last_y = 0.0;
    bool first_mouse = true;
};
void ui_init(UIState& state, int width, int height);
void ui_update_input(UIState& state, GLFWwindow* window, float dt);
void ui_compute_matrices(UIState& state, int width, int height);
void ui_draw(UIState& state);
