#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inCol;

layout(location = 0) out vec2 outUV;

void main() {
    gl_Position = vec4(inPos, 1.0); // Set fragment color
    outUV = inCol.rg;
}