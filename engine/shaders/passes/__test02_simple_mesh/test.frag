#version 450

layout(location = 0) in vec3 posWS;

// output write
layout(location = 0) out vec4 outFragColor;

void main() { outFragColor = vec4(fract(posWS), 1); }