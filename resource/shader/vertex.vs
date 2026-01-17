#version 330 core

// 顶点属性输入 (严格匹配 buffer.cpp location 设置)
layout (location = 0) in vec3 aPos;    // 顶点局部坐标
layout (location = 1) in vec3 aNormal; // 顶点法线，用于描边位移与光照计算
layout (location = 2) in vec2 aUV0;    // 第一套 UV 坐标，用于纹理采样

// 传往片元着色器的变量
out vec3 FragPos;  
out vec3 Normal;   
out vec2 TexCoord; 

// 坐标变换矩阵 (由外部程序传入)
uniform mat4 model;      
uniform mat4 view;       
uniform mat4 projection; 

// 描边控制参数
uniform bool isOutline;     // 是否开启描边渲染模式
uniform float outlineWidth; // 描边粗细（顶点沿法线方向外扩的距离）

void main()
{
    vec3 pos = aPos;
    
    // 【功能：描边位移】
    // 若处于描边模式，顶点坐标沿法线方向偏移
    if (isOutline) {
        pos += aNormal * outlineWidth;
    }

    // 计算世界空间位置与法线
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoord = aUV0; 
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
