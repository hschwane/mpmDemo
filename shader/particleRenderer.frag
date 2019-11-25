#version 450

in float radius;
in vec4 vel_color;

uniform float brightness=1;
uniform bool falloff=false;

out vec4 fragment_color;

void main()
{
    vec2 coord = (gl_PointCoord - vec2(0.5));
    float distFromCenter = length(coord);

    // make it round
    if(distFromCenter > 0.5)
        discard;

    vec4 color = vel_color;

    if(falloff)
        color *= (1-distFromCenter);

	fragment_color = color*brightness;
}