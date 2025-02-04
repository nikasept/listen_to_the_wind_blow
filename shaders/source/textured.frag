#version 450 core

layout(location = 0) in vec2 inUV; // Interpolated color from vertex shader

// uniform time, why is it done like this? investigate
layout(set = 2, binding = 0) uniform sampler2D tex;
layout(set = 2, binding = 1) uniform sampler Sampler;

layout(location = 0) out vec4 FragColor; // Final fragment color

void main() {
    //FragColor = vec4(inUV, 0.0f, 1.0f) + texture(tex, inUV);
    FragColor = texture(tex, inUV);
}