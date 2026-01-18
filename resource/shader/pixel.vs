#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

[cite_start]
uniform sampler2D texture1;  

// --- 新增：外部光照模型参数 ---
uniform vec3 lightDir;      // 光照方向
uniform float lightIntensity; // 光照强度
uniform vec3 lightColor;    // 光源颜色
uniform vec3 viewPos;       // 相机位置
uniform bool isOutline;     // 描边标识

void main() {
    // 【功能实现：描边颜色】
    // 膨胀的边缘描成黑色
    if (isOutline) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    [cite_start]// 1. 贴图采样 [cite: 2]
    vec4 texColor = texture(texture1, TexCoords);

    // 2. 【功能实现：完善光照模型】
    vec3 norm = normalize(Normal);
    vec3 L = normalize(-lightDir); // 光源向量
    
    // 环境光 (Ambient)
    vec3 ambient = 0.2 * lightColor;
    
    // 漫反射 (Diffuse)
    float diff = max(dot(norm, L), 0.0);
    vec3 diffuse = diff * lightColor * lightIntensity;
    
    // 镜面反射 (Specular / Blinn-Phong)
    vec3 V = normalize(viewPos - FragPos);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(norm, H), 0.0), 32.0);
    vec3 specular = spec * lightColor * lightIntensity;

    // 3. 颜色合成
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}
