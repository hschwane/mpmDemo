/*
 * mpmDemo
 * mpmSolver2D.h
 *
 * @author: Hendrik Schwanekamp
 * @mail:   hendrik.schwanekamp@gmx.net
 *
 * Implements the mpmSolver2D class
 *
 * Copyright (c) 2019 Hendrik Schwanekamp
 *
 */

#ifndef MPMDEMO_MPMSOLVER2D_H
#define MPMDEMO_MPMSOLVER2D_H

// includes
//--------------------
#include <mpUtils/mpUtils.h>
#include <mpUtils/mpGraphics.h>
//--------------------

//-------------------------------------------------------------------
/**
 * class mpmSolver2D
 *
 * usage:
 *
 */
class mpmSolver2D
{
public:

    explicit mpmSolver2D(int width, int height); //!< constructor of the mpm solver
    void setWindowSize(int width, int height); //!< set new aspect ratio for the simulation domain (will resize the grid)

    void applyExternalForce(glm::vec2 force); //!< apply an external force on the material
    void addParticles(glm::vec2 position, float radius); //!< add particles in radius around position
    void addCollisionObject(glm::vec2 position, float radius); //!< add collision object in radius around position

    void drawUI(bool* shouldBeDrawn); //!< show the user interface
    void advanceSimulation(); //!< advance the simulation
    void drawCollisionMap(); //!< draw the collision map
    void drawParticles(); //!< draw the particles

private:

    // settings
    int m_gridHeight{256}; //!< number of grid cells in Y direction
    int m_paticleBufferResizeValue{4096}; //!< number of slots added to particle buffer upon reallocation

    float m_timestep{0.01}; //!< length of a timestep
    int timestepsPerFrame{1}; //!< number of timesteps to be performed per frame

    float m_particleSpawnSeperation{1};
    float m_particleMass{0.001}; //!< mass of one particle
    float m_bulkModulus{5}; //!< bulk modulus of the material
    float m_exponentialGamma{1}; //!< exponential gamme of the material
    glm::vec2 m_gravity{0,-1}; //!< gravity

    float m_particleRenderSize{1.0}; //!< render size of particles
    float m_particleBrightness{1.0}; //!< brightness of particles
    bool m_falloff{true}; //!< render particles with soft edges
    bool m_colorcodeVelocity{false}; //!< colorcode particle velocities
    glm::vec3 m_particleColor{0.098, 0.670, 0.921}; //!< color of particles
    glm::vec3 m_collisionColor{0.921, 0.584, 0.203}; //!< color of the collision boundary
    glm::vec3 m_backgroundColor{ .2f, .2f, .2f}; // color drawn in the background

    // other vars
    glm::vec2 m_additionalForces{0,0}; //!< forces added via apply external force function
    glm::ivec2 m_domainSize; //!< actual amount of grid cells / size of domain
    float m_simDomainScale; //!< scale sim down for rendering
    int m_particleBufferCapacity; //!< current capacity of particle buffers
    int m_numParticles{0}; //!< number of particles in the simulation

    // particle data
    mpu::gph::Buffer<glm::vec2> m_particlePositon; //!< position of particles
    mpu::gph::Buffer<glm::vec2> m_particleVelocity; //!< velocity of particles
    mpu::gph::Buffer<glm::mat2> m_particleF; //!< deformation gradient of particles
    mpu::gph::Buffer<glm::mat2> m_particleAffine; //!< APIC affine matrix
    mpu::gph::Buffer<float> m_initialVolume; //! inital volume of particles

    // grid data
    mpu::gph::Texture m_gridVelocityMass; //!< velocities and mass on the grid (mass is alpha component)
    mpu::gph::Texture m_gridCollision; //!< collision level set on the grid

    // vao for rendering
    mpu::gph::VertexArray m_vao; //!< vao for rendering

    // shader
    mpu::gph::ShaderProgram m_particleRenderShader; //!< shader to draw particles
    mpu::gph::ShaderProgram m_addParticlesShader; //!< adds a patch of particles
    mpu::gph::ShaderProgram m_addCollisionObjectShader; //!< adds a patch of collision object
    mpu::gph::ShaderProgram m_copyCollisionMapShader; //!< copy collision map from one texture to another
    mpu::gph::ScreenFillingTri m_collisionMapRenderer; //!< renders collision map

    mpu::gph::ShaderProgram m_p2gShader; //!< shader used during particle to grid transfer
    mpu::gph::ShaderProgram m_gridUpdateShader; //!< shader used for grid update
    mpu::gph::ShaderProgram m_g2pShader; //!< shader used during grid to particle transfer

    void clearCollisionMap(); //!< clears the collision map
};


#endif //MPMDEMO_MPMSOLVER2D_H
