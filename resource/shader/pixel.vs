#version 330 core

// ---------------------------------------------------------
// 输入变量 (来自顶点着色器)
// ---------------------------------------------------------
in vec3 FragPos;      // 片段在世界空间中的位置
in vec3 Normal;       // 片段的世界空间法线
in vec2 TexCoords;    // 纹理坐标
in float vIsOutline;  // 描边标记 (1.0 表示是描边片段)

// ---------------------------------------------------------
// 输出变量
// ---------------------------------------------------------
out vec4 FragColor;   // 最终输出颜色

// ---------------------------------------------------------
// Uniform 变量
// ---------------------------------------------------------
uniform sampler2D texture1;    // 漫反射纹理
uniform vec3 lightPos;         // 光源位置 (世界空间)
uniform vec3 lightColor;       // 光源颜色
uniform samplerCube shadowMap; // 立方体阴影贴图 (用于点光源阴影)
uniform float farPlane;        // 阴影投影的远平面距离 (用于深度归一化)

void main() {
    // ---------------------------------------------------------
    // 1. 描边处理
    // ---------------------------------------------------------
    if (vIsOutline > 0.5) {
        // 如果是描边片段，直接输出纯黑色
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // ---------------------------------------------------------
    // 2. 基础数据准备
    // ---------------------------------------------------------
    vec4 texColor = texture(texture1, TexCoords);
    vec3 norm = normalize(Normal);
    vec3 lightDir = lightPos - FragPos;
    float distanceToLight = length(lightDir);
    vec3 L = lightDir / distanceToLight; // 指向光源的单位向量
    
    // ---------------------------------------------------------
    // 3. 漫反射计算 (Lambertian)
    // ---------------------------------------------------------
    float diff = max(dot(norm, L), 0.0); // 注意：这里使用了 -norm

    // ---------------------------------------------------------
    // 4. 环境光计算
    // ---------------------------------------------------------
    vec3 ambient = 0.5 * lightColor * texColor.rgb;

    // ---------------------------------------------------------
    // 5. 阴影计算 (PCF - Percentage Closer Filtering)
    // ---------------------------------------------------------
    float shadow = 0.0;
    float bias = 0.05;       // 阴影偏移，防止阴影失真 (Shadow Acne)
    int samples = 20;        // 采样次数
    float viewDistance = length(lightPos - FragPos);
    float diskRadius = 0.01; // 采样半径
    vec3 fragToLight = FragPos - lightPos;
    float currentDepth = length(fragToLight);
    vec3 normFragToLight = normalize(fragToLight);
    
    // 采样偏移方向数组 (用于在球面上随机采样以柔化阴影边缘)
    vec3 sampleOffsetDirections[20] = vec3[]
    (
       vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
       vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
       vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    // 执行 PCF 采样
    for(int i = 0; i < samples; ++i)
    {
        // 从阴影贴图中获取最近深度值
        float closestDepth = texture(shadowMap, normFragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= farPlane; // 反归一化 [0,1] -> [0, farPlane]
        
        // 比较当前深度与最近深度
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples); // 计算平均阴影值 [0, 1]

    // ---------------------------------------------------------
    // 6. 最终颜色合成
    // ---------------------------------------------------------
    float lighting = diff;
    // 最终颜色 = 环境光 + (1 - 阴影) * 漫反射
    vec3 result = ambient + 0.5 * (1.0 - shadow) * lighting * lightColor * texColor.rgb;
    
    FragColor = vec4(result, texColor.a);
}
