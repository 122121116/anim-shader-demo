#version 330 core

// ---------------------------------------------------------
// 顶点属性 (Layout Locations)
// ---------------------------------------------------------
layout(location = 0) in vec3 aPos;    // 顶点位置
layout(location = 1) in vec3 aNormal; // 顶点法线
layout(location = 2) in vec3 color;   // 顶点颜色

// ---------------------------------------------------------
// Uniform 变量
// ---------------------------------------------------------
uniform mat4 model;      // 模型矩阵：局部 -> 世界
uniform mat4 view;       // 视图矩阵：世界 -> 观察
uniform mat4 projection; // 投影矩阵：观察 -> 裁剪

// ---------------------------------------------------------
// 输出变量 (传递给片段着色器)
// ---------------------------------------------------------
out vec3 FragPos;   // 片段的世界空间位置
out vec3 Normal;    // 片段的世界空间法线
out vec3 CubeColor; // 传递的颜色

void main() {
    // 1. 计算世界空间位置
    vec4 worldPos = model * vec4(aPos, 1.0);
    
    // 2. 计算法线矩阵并变换法线
    // 法线矩阵是模型矩阵左上角 3x3 部分的逆转置矩阵
    // 用于正确处理非均匀缩放下的法线变换
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 worldNormal = normalize(normalMatrix * aNormal);
    
    // 3. 传递数据给片段着色器
    vec4 finalPos = worldPos;
    FragPos = finalPos.xyz;
    Normal = worldNormal;
    CubeColor = color;
    
    // 4. 计算最终裁剪空间坐标
    gl_Position = projection * view * finalPos;
}
