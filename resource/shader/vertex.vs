#version 330 core
[cite_start]
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// --- 新增：描边控制参数 ---
uniform bool isOutline;     // 描边开关
uniform float outlineWidth; // 膨胀宽度

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    vec3 pos = aPos;

    // 【功能实现：背面膨胀法】
    // 描边阶段将顶点沿法线挤出
    if (isOutline) {
        pos += aNormal * outlineWidth;
    }

    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
