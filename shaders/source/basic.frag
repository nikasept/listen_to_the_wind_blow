#version 450 core

// Specify location for input variable
layout(location = 0) in vec3 vColor;    // Interpolated color from vertex shader

// Specify location for output variable
layout(location = 0) out vec4 FragColor; // Final fragment color

void main()
{
    FragColor = vec4(vColor, 1.0); // Set fragment color
}