#version 330 core

// ---------------------------------------------------------
// 输入变量 (来自顶点着色器)
// ---------------------------------------------------------
in vec3 FragPos;      // 片段在世界空间中的位置
in vec3 Normal;       // 片段的世界空间法线
in vec3 CubeColor;    // 顶点传递的颜色 (本例中主要使用 uniform objectColor)

// ---------------------------------------------------------
// 输出变量
// ---------------------------------------------------------
out vec4 FragColor;   // 最终输出颜色

// ---------------------------------------------------------
// Uniform 变量
// ---------------------------------------------------------
uniform vec3 lightPos;         // 光源位置
uniform vec3 lightColor;       // 光源颜色
uniform vec3 objectColor;      // 物体基础颜色
uniform samplerCube shadowMap; // 立方体阴影贴图
uniform float farPlane;        // 远平面距离

void main() {
    // ---------------------------------------------------------
    // 1. 基础数据准备
    // ---------------------------------------------------------
    vec3 norm = normalize(Normal);
    vec3 lightDir = lightPos - FragPos;
    float distanceToLight = length(lightDir);
    vec3 L = lightDir / distanceToLight;
    
    // ---------------------------------------------------------
    // 2. 漫反射计算
    // ---------------------------------------------------------
    float diff = max(dot(norm, L), 0.0);

    // ---------------------------------------------------------
    // 3. 环境光计算
    // ---------------------------------------------------------
    vec3 ambient = 0.5 * lightColor * objectColor;

    // ---------------------------------------------------------
    // 4. 阴影计算 (PCF)
    // ---------------------------------------------------------
    float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;
    float viewDistance = length(lightPos - FragPos);
    float diskRadius = 0.01;
    vec3 fragToLight = FragPos - lightPos;
    float currentDepth = length(fragToLight);
    vec3 normFragToLight = normalize(fragToLight);
    
    // PCF 采样方向
    vec3 sampleOffsetDirections[20] = vec3[]
    (
       vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
       vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
       vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowMap, normFragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= farPlane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    // ---------------------------------------------------------
    // 5. 最终颜色合成
    // ---------------------------------------------------------
    float lighting = diff;
    // 最终颜色 = 环境光 + (1 - 阴影) * 漫反射
    vec3 result = ambient + 0.5 * (1.0 - shadow) * lighting * lightColor * objectColor;
    
    FragColor = vec4(result, 1.0);
}
