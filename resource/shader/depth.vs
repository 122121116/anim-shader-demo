#version 330 core

// ---------------------------------------------------------
// 阴影深度贴图生成的顶点着色器
// ---------------------------------------------------------

layout(location = 0) in vec3 aPos; // 顶点位置

uniform mat4 model;            // 模型矩阵
uniform mat4 lightSpaceMatrix; // 光空间矩阵


out vec4 FragPos; // 输出世界空间位置

void main() {
    // 计算世界空间位置
    FragPos = model * vec4(aPos, 1.0);
    

    gl_Position = lightSpaceMatrix * FragPos;
}
