#version 450

layout(location = 0) in vec3 posCS;

// output write
layout(location = 0) out vec4 outFragColor;

layout(push_constant) uniform constants
{
    mat4 RenderMatrix;
    vec4 CameraPosWS;
    vec4 DebugColor;
}
PushConstants;

void main() { outFragColor = PushConstants.DebugColor; }