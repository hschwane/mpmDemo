#version 450

// --------------------------------------
// uniforms

// --------------------------------------
// buffer bindings
layout(binding=2,std430) buffer ParticlePosition { vec2 particlePosition[]; };
layout(binding=3,std430) buffer ParticleVelocity { vec2 particleVelocity[]; };
layout(binding=4,std430) buffer ParticleF { mat2 particleF[]; };
layout(binding=5,std430) buffer ParticleAffine { mat2 particleAffine[]; };
layout(binding=6,std430) buffer ParticleVolume { float particleVolume[]; };

// --------------------------------------
// out
out vec4 momentum_mass;

void main()
{


    momentum_mass = vec4(0.1,0.1,0.1,1);
}