#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;   // 灯光位置
uniform vec3 viewPos;    // 相机（观察者）位置
uniform vec3 lightColor; // 灯光颜色
uniform vec3 objectColor;// 物体颜色

void main() {
    // 1. 环境光 (Ambient) - 保证没光的地方不是纯黑
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // 2. 漫反射 (Diffuse) - 模拟光直接照射的效果
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 3. 镜面反射 (Specular) - 模拟高光
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
