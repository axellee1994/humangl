#version 400 core

in vec3 fragPos;
in vec3 normal;

uniform vec3 lightPos;
uniform vec3 color;

out vec4 fragColor;

void main() {
    float ambientStrength = 0.25;
    vec3  ambient = ambientStrength * color;

    vec3  norm = normalize(normal);
    vec3  lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3  diffuse = diff * color;

    vec3 result = ambient + diffuse;
    fragColor = vec4(result, 1.0);
}
