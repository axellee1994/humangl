#version 400 core

in vec2 texCoord;

uniform sampler2D fontTex;
uniform vec3 textColor;

out vec4 fragColor;

void main() {
    float a = texture(fontTex, texCoord).r;
    if (a < 0.5)
        discard;
    fragColor = vec4(textColor, 1.0);
}
