#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float vIsOutline;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 lightDir;
uniform float lightIntensity;

void main() {
    if (vIsOutline > 0.5) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // 描边设为黑色
        return;
    }

    vec4 texColor = texture(texture1, TexCoords); // 采样 texture1
    vec3 norm = normalize(Normal);
    vec3 L = normalize(-lightDir);
    
    // 基础光照模型：环境光 + 漫反射
    float diff = max(dot(norm, L), 0.0);
    vec3 result = (0.2 + diff * lightIntensity) * texColor.rgb;
    
    FragColor = vec4(result, texColor.a);
}
