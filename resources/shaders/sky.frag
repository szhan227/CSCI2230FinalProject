#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragColor;

uniform float ka;

uniform float kd;
uniform vec4 lightDir;

uniform float ks;
uniform float shininess;
uniform vec4 cameraPosition;

void main() {
    fragColor = vec4(0.78, 0.88, 1.0, 1.0); // Light blue color
}