/*
 * mpmDemo
 * mpmSolver2D.cpp
 *
 * @author: Hendrik Schwanekamp
 * @mail:   hendrik.schwanekamp@gmx.net
 *
 * Implements the mpmSolver2D class
 *
 * Copyright (c) 2019 Hendrik Schwanekamp
 *
 */

// includes
//--------------------
#include "mpmSolver2D.h"
//--------------------

// function definitions of the mpmSolver2D class
//-------------------------------------------------------------------
mpmSolver2D::mpmSolver2D(float aspectRatio)
    : m_gridSize(aspectRatio*m_gridHeight,m_gridHeight),
      m_simDomainScale(1.0f/(m_gridHeight/2.0f)),
      m_gridVelocityMass(mpu::gph::TextureTypes::texture2D, GL_RGBA16F, 1, m_gridSize.x, m_gridSize.y),
      m_gridCollision(mpu::gph::TextureTypes::texture2D, GL_R16F, 1, m_gridSize.x, m_gridSize.y),
      m_particleBufferCapacity(m_paticleBufferResizeValue),
      m_particlePositon(m_particleBufferCapacity),
      m_particleVelocity(m_particleBufferCapacity),
      m_particleF(m_particleBufferCapacity),
      m_particleAffine(m_particleBufferCapacity),
      m_initialVolume(m_particleBufferCapacity)
{
    // bind all the buffers
    m_particlePositon.bindBase(0,GL_SHADER_STORAGE_BUFFER);
    m_particleVelocity.bindBase(1,GL_SHADER_STORAGE_BUFFER);
    m_particleF.bindBase(2,GL_SHADER_STORAGE_BUFFER);
    m_particleAffine.bindBase(3,GL_SHADER_STORAGE_BUFFER);
    m_initialVolume.bindBase(4,GL_SHADER_STORAGE_BUFFER);

    // bind all the images
    m_gridVelocityMass.bindImage(0,GL_READ_WRITE, GL_RGBA32F);
    m_gridCollision.bindImage(1,GL_READ_WRITE, GL_RGBA32F);

    // bind collision map as texture
    m_gridCollision.bind(2);

}

void mpmSolver2D::setAspectRation(float aspectRatio)
{

}

void mpmSolver2D::applyExternalForce(glm::vec2 force)
{
    m_additionalForces += force;
}

void mpmSolver2D::addParticles(glm::vec2 position, float radius)
{

}

void mpmSolver2D::addCollisionObject(glm::vec2 position, float radius)
{

}

void mpmSolver2D::drawUI(bool* shouldBeDrawn)
{

}

void mpmSolver2D::advanceSimulation()
{

}

void mpmSolver2D::drawCollisionMap()
{

}

void mpmSolver2D::drawParticles()
{

}
