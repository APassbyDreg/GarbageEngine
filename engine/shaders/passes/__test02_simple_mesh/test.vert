#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texcoord;
layout(location = 4) in int flags;

layout(location = 0) out vec3 posWS;

void main()
{
    posWS = position;

    // output the position of each vertex
    gl_Position = vec4(position, 1.0f);
}