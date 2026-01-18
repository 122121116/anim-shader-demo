#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineWidth; // 0.02-0.05
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float vIsOutline;

void main() {
    // 计算世界空间位置和法线
    vec4 worldPos = model * vec4(aPos, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 worldNormal = normalize(normalMatrix * aNormal);
    
    // 视线方向（世界空间）
    vec3 viewDir = normalize(viewPos - worldPos.xyz);
    
    // 判断是否为背面（膨胀部分）
    float ndotv = dot(worldNormal, viewDir);
    
    vec4 finalPos = worldPos;
    vIsOutline = 0.0;
    
    if (ndotv > 0.0) {
        // 只移动背面顶点，方向沿着法线
        finalPos.xyz += worldNormal * outlineWidth;
        vIsOutline = 1.0;
    }
    
    // 输出到片段着色器
    FragPos = finalPos.xyz;
    Normal = worldNormal;
    TexCoords = aTexCoords;
    
    // 转换到裁剪空间
    gl_Position = projection * view * finalPos;
}