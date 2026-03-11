#version 400 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 normal;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    fragPos = worldPos.xyz;

    normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * worldPos;
}
