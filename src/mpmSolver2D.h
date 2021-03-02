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

    void applyExternalAcc(glm::vec2 force); //!< apply an external force on the material
    void addParticles(glm::vec2 position); //!< add particles in radius around position
    void addCollisionObject(glm::vec2 position); //!< add collision object in radius around position

    void drawUI(bool* shouldBeDrawn); //!< show the user interface
    void advanceSimulation(); //!< advance the simulation
    void drawCollisionMap(); //!< draw the collision map
    void drawParticles(); //!< draw the particles

private:

    // settings
    int m_gridHeight{512}; //!< number of grid cells in Y direction
    int m_particleBufferResizeValue{4096}; //!< number of slots added to particle buffer upon reallocation

    float m_timestep{0.1}; //!< length of a timestep
    int m_timestepsPerFrame{2}; //!< number of timesteps to be performed per frame
    int m_g2pGroupSize{256}; //!< size of workgroup for g2p
    glm::ivec2 m_gridUpdateGroupSize{16,16}; //!< size of workgroup for gridUpdate

    float m_particleSpawnSeparation{1.0};
    float m_particleMass{0.1}; //!< mass of one particle
    float m_bulkModulus{1}; //!< bulk modulus of the material
    float m_exponentialGamma{1}; //!< exponential gamma of the material
    glm::vec2 m_gravity{0,-10}; //!< gravity

    float m_particleRenderSize{1.0}; //!< render size of particles
    float m_particleBrightness{1.0}; //!< brightness of particles
    bool m_falloff{true}; //!< render particles with soft edges
    bool m_colorcodeVelocity{false}; //!< colorcode particle velocities
    glm::vec3 m_particleColor{0.098, 0.670, 0.921}; //!< color of particles
    glm::vec3 m_collisionColor{0.921, 0.584, 0.203}; //!< color of the collision boundary
    glm::vec3 m_backgroundColor{ .2f, .2f, .2f}; //!< color drawn in the background
    float m_obstacleBrushSize{15}; //!< radius of grid cells effected when drawing obstacles
    float m_particleBrushSize{15}; //!< radius of grid cells effected when drawing particles

    // other vars
    glm::vec2 m_additionalAcc{0, 0}; //!< acceleration added via apply external acc function
    glm::ivec2 m_domainSize; //!< actual amount of grid cells == size of domain
    float m_simDomainScale; //!< scale sim down for rendering
    int m_particleBufferCapacity; //!< current capacity of particle buffers
    int m_numParticles{0}; //!< number of particles in the simulation
    int m_currentWidth, m_currentHeight; //!< current width and height of the window

    // particle data
    mpu::gph::Buffer<glm::vec2> m_particlePositon; //!< position of particles
    mpu::gph::Buffer<glm::vec2> m_particleVelocity; //!< velocity of particles
    mpu::gph::Buffer<glm::mat2> m_particleF; //!< deformation gradient of particles
    mpu::gph::Buffer<glm::mat2> m_particleAffine; //!< APIC affine matrix
    mpu::gph::Buffer<float> m_initialVolume; //! initial volume of particles

    // grid data
    mpu::gph::Texture m_gridVelX; //!< x-velocities on the grid
    mpu::gph::Texture m_gridVelY; //!< y-velocities on the grid
    mpu::gph::Texture m_gridMass; //!<  mass on the grid
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
    mpu::gph::ShaderProgram m_p2gShaderAtomic; //!< shader used during particle to grid transfer
    mpu::gph::ShaderProgram m_gridUpdateShader; //!< shader used for grid update
    mpu::gph::ShaderProgram m_g2pShader; //!< shader used during grid to particle transfer

    mpu::gph::Framebuffer m_fbo; //!< fbo used for particle to grid

    void clearCollisionMap(); //!< clears the collision map
};


#endif //MPMDEMO_MPMSOLVER2D_H
