#version 450 core
// Vertex attributes from the vertex buffer

// Output variables to the fragment shader
layout(location = 0) out vec3 outColors;

const vec2 vertexPositions[3] = vec2[3] (
    vec2(0.0, 1.0),
    vec2(-1.0, 0.0),
    vec2(1.0, 0.0)
);

const vec3 vertexColors[3] = vec3[3] (
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main()
{
    // Pass the color to the fragment shader

    gl_Position = vec4(vertexPositions[gl_VertexIndex], 0.0f, 1.0f);

    // Set the vertex position in clip space
    outColors = vertexColors[gl_VertexIndex];
}