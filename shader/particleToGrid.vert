#version 450

#include "math.glsl"

layout(location=0) in vec4 input_position;

uniform vec2 simDomain;

out flat int id;

void main()
{
    vec2 pos = input_position.xy;
    pos.x /= float(simDomain.x);
    pos.y /= float(simDomain.y);
    pos -= vec2(0.5,0.5);
    pos *=2;
    gl_Position =  vec4(pos,0,1);
    gl_PointSize = 3;
    id = gl_VertexID;
}