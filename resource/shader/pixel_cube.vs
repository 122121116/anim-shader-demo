#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 CubeColor;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 L = normalize(-lightDir);
    
    float diff = max(dot(norm, L), 0.0);

    vec3 result = (0.2+diff) * lightColor * CubeColor;
    
    FragColor = vec4(result, 1.0);
}