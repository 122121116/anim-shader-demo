#version 330 core
in vec3 WorldPos;

uniform vec3 lightPos;
uniform float far_plane;

void main() {
    float lightDistance = length(WorldPos - lightPos);
    lightDistance = lightDistance / far_plane;
    gl_FragDepth = lightDistance;
}

