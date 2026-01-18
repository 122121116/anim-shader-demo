#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineWidth; // 推荐 0.02

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float vIsOutline; // 自动计算标记

void main() {
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));
    vec3 worldNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    
    // 假设相机大致位置计算视线方向
    vec3 viewDir = normalize(vec3(0.0, 0.0, 5.0) - worldPos); 

    // 背面膨胀判定逻辑
    float ndotv = dot(worldNormal, viewDir);
    vec3 pos = aPos;
    
    if (ndotv < 0.1) { 
        pos += aNormal * outlineWidth; // 仅膨胀边缘/背面
        vIsOutline = 1.0;
    } else {
        vIsOutline = 0.0;
    }

    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = worldNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
