#version 400 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform vec2 screenSize;

out vec2 texCoord;

void main() {
    vec2 ndc = (aPos / screenSize) * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
    texCoord = aTexCoord;
}
