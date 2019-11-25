#version 450

#include "math.glsl"

layout(location=0) in vec4 input_position;

void main()
{
    gl_PointSize = 3;
    gl_Position =  input_position;
}