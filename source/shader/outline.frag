#version 330 core

uniform vec4 outlineColor;

in vec3 fragNormal;

out vec4 fragColor;

void main() {
    // Set the color of the outline
    fragColor = outlineColor;
}
