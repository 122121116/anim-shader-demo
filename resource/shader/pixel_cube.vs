#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 CubeColor;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 pointLightColor;
uniform samplerCube shadowMap;
uniform vec3 lightPos;
uniform float far_plane;
uniform bool useShadow;

float calcShadow(vec3 fragPos) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float closestDepth = texture(shadowMap, fragToLight).r * far_plane;
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 L = normalize(-lightDir);
    float diff = max(dot(norm, L), 0.0);
    vec3 result = diff * lightColor * CubeColor;
    float shadow = 0.0;
    if (useShadow) {
        shadow = calcShadow(FragPos);
    }
    result += (1.0 - shadow) * pointLightColor;
    FragColor = vec4(result, 1.0);
}
