#version 330 core

// ---------------------------------------------------------
// 阴影深度贴图生成的片段着色器
// ---------------------------------------------------------

in vec4 FragPos; // 世界空间位置 (来自顶点着色器)

uniform vec3 lightPos;  // 光源位置
uniform float farPlane; // 远平面距离 (用于归一化深度)

void main() {
    // 计算片段到光源的距离
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // 将距离归一化到 [0, 1] 范围
    // 这是为了将其存储在深度纹理中
    lightDistance = lightDistance / farPlane;
    
    // 写入深度值
    gl_FragDepth = lightDistance;
}
