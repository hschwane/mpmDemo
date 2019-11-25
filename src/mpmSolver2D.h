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

    explicit mpmSolver2D(float aspectRatio); //!< constructor of the mpm solver
    void setAspectRation(float aspectRatio); //!< set new aspect ratio for the simulation domain (will resize the grid)

    void applyExternalForce(glm::vec2); //!< apply an external force on the material
    void addParticles(glm::vec2 position, float radius); //!< add particles in radius around position
    void addCollisionObject(glm::vec2 position, float radius); //!< add collision object in radius around position

    void drawUI(bool* shouldBeDrawn); //!< show the user interface
    void advanceSimulation(); //!< advance the simulation
    void drawCollisionMap(); //!< draw the collision map
    void drawParticles(); //!< draw the particles

private:

    // settings
    int m_gridHight{256}; //!< number of grid cells in Y direction

    float m_timestep{0.01}; //!< length of a timestep
    int timestepsPerFrame{1}; //!< number of timesteps to be performed per frame

    float m_particleMass{0.001}; //!< mass of one particle
    float m_bulkModulus{5}; //!< bulk modulus of the material
    float m_exponentialGamma{1}; //!< exponential gamme of the material
    glm::vec2 m_gravity{0,-1}; //!< gravity

    float m_particleRenderSize{0.01}; //!< render size of particles
    glm::vec3 m_particleColor{0.098, 0.670, 0.921}; //!< color of particles
    glm::vec3 m_collisionColor{0.921, 0.584, 0.203}; //!< color of the collision boundary

    // other vars
    glm::vec2 m_additionalForces{0,0}; //!< forces added via apply external force function
    glm::ivec2 m_gridSize; //!< actual amount of grid cells
    float m_gridCellSize; //!< size of a grid cell in simulation units
    float m_Mfactor; //!< scaling factor from the apic equation

    // particle data
    int m_numParticles; //!< number of particles in the simulation
    mpu::gph::Buffer<glm::vec2> m_particlePositon; //!< position of particles
    mpu::gph::Buffer<glm::vec2> m_particleVelocity; //!< velocity of particles
    mpu::gph::Buffer<glm::mat2> m_particleF; //!< deformation gradient of particles
    mpu::gph::Buffer<glm::mat2> m_particleAffine; //!< APIC affine matrix
    mpu::gph::Buffer<float> m_initialVolume; //! inital volume of particles

    // grid data
    mpu::gph::Texture m_gridMass; //!< masses on the grid
    mpu::gph::Texture m_gridVelocity; //!< velocities on the grid
    mpu::gph::Texture m_gridCollision; //!< collision level set on the grid

    // shader
    mpu::gph::ShaderProgram m_particleRenderShader; //!< shader to draw particles
    mpu::gph::ShaderProgram m_collisionMapRenderShader; //!< shader to draw the collision map

    mpu::gph::ShaderProgram m_p2gShader; //!< shader used during particle to grid transfer
    mpu::gph::ShaderProgram m_gridUpdateShader; //!< shader used for grid update
    mpu::gph::ShaderProgram m_g2pShader; //!< shader used during grid to particle transfer
};


#endif //MPMDEMO_MPMSOLVER2D_H
