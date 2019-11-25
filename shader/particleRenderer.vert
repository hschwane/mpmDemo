#version 450

#include "math.glsl"

layout(location=0) in vec4 input_position;
layout(location=1) in vec4 input_velocity;

uniform float renderScale;
uniform float renderSize;
uniform vec3 defaultColor;
uniform bool colorcodeVelocity=false;
uniform vec2 domainSize;

out float radius;
out vec4 vel_color;

void main()
{
    vec2 pos = input_position.xy;
    pos.x /= domainSize.x;
    pos.y /= domainSize.y;
    pos -= vec2(0.5,0.5);
    pos *=2;

	gl_Position =  vec4(pos,0,1);
    gl_PointSize = renderScale * renderSize;

    if(colorcodeVelocity)
    {
        if (iszero(input_velocity.xyz))
        vel_color = vec4(defaultColor,1);
        else
        {
            vec3 nv = 0.5f*normalize(input_velocity.xyz)+vec3(0.5f);
            vel_color = vec4(nv, 1);
        }
    }
    else
        vel_color = vec4(defaultColor,1);

	radius = gl_PointSize / 2;
}