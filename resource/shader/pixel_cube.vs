#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 CubeColor;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform samplerCube shadowMap;
uniform float farPlane;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = lightPos - FragPos;
    float distanceToLight = length(lightDir);
    vec3 L = lightDir / distanceToLight;
    
    float diff = max(dot(norm, L), 0.0);

    vec3 ambient = 0.3 * lightColor * objectColor;

    float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;
    float viewDistance = length(lightPos - FragPos);
    float diskRadius = 0.01;
    vec3 fragToLight = FragPos - lightPos;
    float currentDepth = length(fragToLight);
    vec3 normFragToLight = normalize(fragToLight);
    
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

    float lighting = diff;

    vec3 result = ambient + 0.7 * (1.0 - shadow) * lighting * lightColor * objectColor;
    
    FragColor = vec4(result, 1.0);
}
