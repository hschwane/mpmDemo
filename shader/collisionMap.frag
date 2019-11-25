#version 330 core

in vec2 texCoord;
out vec4 fragColor;

uniform vec3 color;
uniform vec3 bgcolor;
uniform sampler2D collisionMap;

void main()
{
    float texValue = texture(collisionMap,texCoord).x;
    if(texValue < 0.1)
        fragColor = vec4(color*clamp(abs(texValue),0.3,1),1);
    else
        fragColor = vec4(bgcolor,1);
}