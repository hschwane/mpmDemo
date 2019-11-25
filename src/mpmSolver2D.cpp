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
mpmSolver2D::mpmSolver2D(int width, int height)
    : m_domainSize((float(width) / float(height)) * m_gridHeight, m_gridHeight),
      m_simDomainScale(float(height)/(m_gridHeight)),
      m_gridVelocityMass(mpu::gph::TextureTypes::texture2D, GL_RGBA16F, 1, m_domainSize.x, m_domainSize.y),
      m_gridCollision(mpu::gph::TextureTypes::texture2D, GL_R16F, 1, m_domainSize.x, m_domainSize.y),
      m_particleBufferCapacity(m_paticleBufferResizeValue),
      m_particlePositon(m_particleBufferCapacity),
      m_particleVelocity(m_particleBufferCapacity),
      m_particleF(m_particleBufferCapacity),
      m_particleAffine(m_particleBufferCapacity),
      m_initialVolume(m_particleBufferCapacity),
      m_addCollisionObjectShader({{PROJECT_SHADER_PATH"addCollisionObject.comp"}}),
      m_copyCollisionMapShader({{PROJECT_SHADER_PATH"copyCollisionMap.comp"}})
{
    // bind all the buffers
    m_particlePositon.bindBase(0,GL_SHADER_STORAGE_BUFFER);
    m_particleVelocity.bindBase(1,GL_SHADER_STORAGE_BUFFER);
    m_particleF.bindBase(2,GL_SHADER_STORAGE_BUFFER);
    m_particleAffine.bindBase(3,GL_SHADER_STORAGE_BUFFER);
    m_initialVolume.bindBase(4,GL_SHADER_STORAGE_BUFFER);

    // bind all the images
    m_gridVelocityMass.bindImage(0,GL_READ_WRITE, GL_RGBA32F);
    m_gridCollision.bindImage(1,GL_READ_WRITE, GL_R16F);

    // bind collision map as texture
    m_gridCollision.bind(2);

    // clear texture
    clearCollisionMap();

    // set up uniforms
    m_addCollisionObjectShader.uniform1i("collisionImage",1);
    m_copyCollisionMapShader.uniform1i("collisionImageSrc",1);
    m_copyCollisionMapShader.uniform1i("collisionImageDst",3);

    // set up shader for collision map renderer
    m_collisionMapRenderer.setScreenFillShader(PROJECT_SHADER_PATH"collisionMap.frag");
    m_collisionMapRenderer.shader().uniform1i("collisionMap",2);
    m_collisionMapRenderer.shader().uniform3f("color",m_collisionColor);
    m_collisionMapRenderer.shader().uniform3f("bgcolor",m_backgroundColor);

}

void mpmSolver2D::setWindowSize(int width, int height)
{
    m_domainSize = glm::vec2((float(width) / float(height)) * m_gridHeight, m_gridHeight);
    m_simDomainScale = float(height)/(m_gridHeight);
    m_gridVelocityMass = mpu::gph::Texture(mpu::gph::TextureTypes::texture2D, GL_RGBA16F, 1, m_domainSize.x, m_domainSize.y);
    mpu::gph::Texture newCollisionMap(mpu::gph::TextureTypes::texture2D, GL_R16F, 1, m_domainSize.x, m_domainSize.y);

    // copy old collision map
    newCollisionMap.bindImage(3,GL_WRITE_ONLY, GL_R16F);
    m_copyCollisionMapShader.dispatch(m_domainSize/2, {16, 16});
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    m_gridCollision = std::move(newCollisionMap);

    // rebind all the images
    m_gridVelocityMass.bindImage(0,GL_READ_WRITE, GL_RGBA32F);
    m_gridCollision.bindImage(1,GL_READ_WRITE, GL_R16F);

    // rebind collision map as texture
    m_gridCollision.bind(2);
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
    position.x *= m_domainSize.x;
    position.y *= m_domainSize.y;
    radius /= m_simDomainScale;
    m_addCollisionObjectShader.uniform2i("addPosition", position);
    m_addCollisionObjectShader.uniform1f("addRadius", radius);

    m_addCollisionObjectShader.dispatch(m_domainSize/2, {16, 16});
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void mpmSolver2D::drawUI(bool* shouldBeDrawn)
{

}

void mpmSolver2D::advanceSimulation()
{

}

void mpmSolver2D::drawCollisionMap()
{
    m_collisionMapRenderer.draw();
}

void mpmSolver2D::drawParticles()
{

}

void mpmSolver2D::clearCollisionMap()
{
    float clear = 1;
    m_gridCollision.clear(&clear,GL_RED,GL_FLOAT,0);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
