#version 330 core
layout(location = 0) in vec3 aPos;    // 顶点位置
layout(location = 1) in vec3 aNormal; // 顶点法线

out vec3 FragPos; // 传给下一个文件的位置信息
out vec3 Normal;  // 传给下一个文件的法线信息

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // 计算像素在世界空间的位置
    FragPos = vec3(model * vec4(aPos, 1.0));
    // 将法线转换到世界空间（防止缩放导致变形）
    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
