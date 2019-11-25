#version 450

#include "kernel.glsl"

// --------------------------------------
// uniforms
uniform float gridCellSize=1;
uniform float pMass;
uniform float apicFactor=4.0f;
uniform float timestep;
uniform float bulkModulus;
uniform float exponentialGamma;

// --------------------------------------
// buffer bindings
layout(binding=2,std430) buffer ParticlePosition { vec2 particlePosition[]; };
layout(binding=3,std430) buffer ParticleVelocity { vec2 particleVelocity[]; };
layout(binding=4,std430) buffer ParticleF { mat2 particleF[]; };
layout(binding=5,std430) buffer ParticleAffine { mat2 particleAffine[]; };
layout(binding=6,std430) buffer ParticleVolume { float particleVolume[]; };


// --------------------------------------
// in
in flat int id;

// --------------------------------------
// out
out vec4 momentum_mass;

void main()
{
    ivec2 gridCell = ivec2(gl_FragCoord.xy);
    vec2 gridCellPos = vec2(gridCell) + vec2(0.5,0.5);

    vec2 pPos = particlePosition[id];
    vec2 pVel = particleVelocity[id];
    mat2 pAffine =  particleAffine[id];
    float pV = particleVolume[id];
    mat2 pF = particleF[id];

    float wij = kernel(gridCellPos,pPos,gridCellSize);
    vec2 xdiff = gridCellPos - pPos;

//    float J = determinant(pF);
//    float pressure = bulkModulus * (1.0f/pow(J,exponentialGamma) -1);
//    mat2 sigma = - pressure * mat2(1.0f);

    // compute force and momentum
//    vec2 force = -  J*pV * apicFactor * sigma * xdiff;
    vec2 momentum = pMass * (pVel + pAffine * xdiff * apicFactor);
//    momentum += force*timestep;

    // rasterization takes care of summation
    momentum_mass = vec4( momentum*wij,pMass*wij,0);
}