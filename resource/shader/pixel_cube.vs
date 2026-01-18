#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 CubeColor;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform samplerCube shadowMap;
uniform float farPlane;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = lightPos - FragPos;
    float distanceToLight = length(lightDir);
    vec3 L = lightDir / distanceToLight;
    
    float diff = max(dot(norm, L), 0.0);

    vec3 ambient = 0.2 * lightColor * CubeColor;

    float shadow = 0.0;
    float bias = 0.05;
    float closestDepth = texture(shadowMap, L).r * farPlane;
    if (distanceToLight - bias > closestDepth) {
        shadow = 1.0;
    }

    float lighting = (1.0 - shadow) * diff;

    vec3 result = ambient + lighting * lightColor * CubeColor;
    
    FragColor = vec4(result, 1.0);
}
