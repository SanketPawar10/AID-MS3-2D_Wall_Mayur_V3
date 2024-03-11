#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;

//uniform mat4 modelMatrix;
//uniform mat3 modelNormalMatrix;
//uniform mat4 modelViewProjection;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float outlineScale;

out vec3 fragNormal;

void main() {
    // Scale vertices along their normals to create the outline effect
    vec3 scaledPosition = vertexPosition + vertexNormal * outlineScale;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(scaledPosition, 1.0);

    fragNormal = vertexNormal;
}
