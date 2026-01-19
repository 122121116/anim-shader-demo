#pragma once
#include "cube.h"
#include <vector>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

struct GLFWwindow;

// 场景中单个立方体的配置信息
struct CubeConfig {
    float length = 1.0f;
    float width = 1.0f;
    float height = 1.0f;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    float scale = 1.0f;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
    bool visible = true;
};

// UI 状态结构体
// 包含所有通过 UI 控制的参数和场景状态
struct UIState {
    bool ui_active = false;      // UI 是否处于激活状态 (鼠标交互中)
    bool input_paused = false;   // 是否暂停输入 (如在编辑框输入时)
    bool p_last = false;         // 上一帧 P 键状态 (用于切换模式)

    float outlinewidth = 0.0001f; // 描边宽度

    // 相机参数
    glm::vec3 camera_pos = glm::vec3(0.0f, 1.5f, 1.0f);
    float camera_yaw = -90.0f;
    float camera_pitch = 0.0f;
    float camera_speed = 2.5f;
    float camera_sensitivity = 0.1f;
    float camera_fov = 60.0f;

    // 模型变换参数
    glm::vec3 model_rot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 model_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    float model_scale = 1.0f;

    // 光源参数 (已弃用，使用 point_light_*)
    glm::vec3 light_pos = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

    // 计算出的变换矩阵
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 view_pos;

    // 点光源参数
    glm::vec3 point_light_pos = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 point_light_color = glm::vec3(1.0f, 1.0f, 1.0f);

    // 立方体相关参数
    glm::mat4 modelcube;
    bool create_cube = false;    // 是否请求创建新立方体
    float cube_length = 1.0f;
    float cube_width = 1.0f;
    float cube_height = 1.0f;
    glm::vec3 cube_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    float cube_scale = 1.0f;
    glm::vec3 cube_color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 cube_rot = glm::vec3(0.0f, 0.0f, 0.0f);
    
    // 场景中所有立方体的列表
    std::vector<CubeConfig> cubes;
    int selected_cube = -1; // 当前选中的立方体索引

    // 鼠标输入状态
    double last_x = 0.0;
    double last_y = 0.0;
    bool first_mouse = true;
};

// 初始化 UI 系统
void ui_init(UIState& state, int width, int height);
// 更新输入状态 (键盘、鼠标)
void ui_update_input(UIState& state, GLFWwindow* window, float dt);
// 计算变换矩阵 (View, Projection, Model)
void ui_compute_matrices(UIState& state, int width, int height);
// 绘制 UI 界面
void ui_draw(UIState& state);
