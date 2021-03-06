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
    : m_domainSize((float(width) / float(height)) * float(m_gridHeight), m_gridHeight),
      m_simDomainScale(float(height)/float(m_gridHeight)),
      m_gridVelX(mpu::gph::TextureTypes::texture2D, GL_R32F, 1, m_domainSize.x, m_domainSize.y),
      m_gridVelY(mpu::gph::TextureTypes::texture2D, GL_R32F, 1, m_domainSize.x, m_domainSize.y),
      m_gridMass(mpu::gph::TextureTypes::texture2D, GL_R32F, 1, m_domainSize.x, m_domainSize.y),
      m_gridCollision(mpu::gph::TextureTypes::texture2D, GL_R16F, 1, m_domainSize.x, m_domainSize.y),
      m_particleBufferCapacity(m_particleBufferResizeValue),
      m_particlePositon(m_particleBufferCapacity),
      m_particleVelocity(m_particleBufferCapacity),
      m_particleF(m_particleBufferCapacity),
      m_particleAffine(m_particleBufferCapacity),
      m_initialVolume(m_particleBufferCapacity),
      m_addCollisionObjectShader({{PROJECT_SHADER_PATH"addCollisionObject.comp"}}),
      m_addParticlesShader({{PROJECT_SHADER_PATH"addParticles.comp"}}),
      m_copyCollisionMapShader({{PROJECT_SHADER_PATH"copyCollisionMap.comp"}}),
      m_particleRenderShader({{PROJECT_SHADER_PATH"particleRenderer.vert"},{PROJECT_SHADER_PATH"particleRenderer.frag"}}),
      m_gridUpdateShader({{PROJECT_SHADER_PATH"gridUpdate.comp"}}),
      m_g2pShader({{PROJECT_SHADER_PATH"g2p.comp"}}),
//      m_p2gShader({{PROJECT_SHADER_PATH"particleToGrid.vert"},{PROJECT_SHADER_PATH"particleToGrid.frag"}}),
      m_p2gShaderAtomic({{PROJECT_SHADER_PATH"p2g.comp"}})
{
    m_currentWidth = width;
    m_currentHeight = height;

    // bind all the buffers
    m_particlePositon.bindBase(2,GL_SHADER_STORAGE_BUFFER);
    m_particleVelocity.bindBase(3,GL_SHADER_STORAGE_BUFFER);
    m_particleF.bindBase(4,GL_SHADER_STORAGE_BUFFER);
    m_particleAffine.bindBase(5,GL_SHADER_STORAGE_BUFFER);
    m_initialVolume.bindBase(6,GL_SHADER_STORAGE_BUFFER);

    // bind all the images
    m_gridVelX.bindImage(3,GL_READ_WRITE, GL_R32F);
    m_gridVelY.bindImage(4,GL_READ_WRITE, GL_R32F);
    m_gridMass.bindImage(5,GL_READ_WRITE, GL_R32F);
    m_gridCollision.bindImage(1,GL_READ_WRITE, GL_R16F);

    // bind collision map as texture
    m_gridCollision.bind(2);

    // clear texture
    clearCollisionMap();

    // set up vao
    m_vao.addAttributeBufferArray(0,0,m_particlePositon,0, sizeof(glm::vec2),2,0);
    m_vao.addAttributeBufferArray(1,1,m_particleVelocity,0, sizeof(glm::vec2),2,0);
    m_vao.bind();

    // set up uniforms
    m_addCollisionObjectShader.uniform1i("collisionImage",1);
    m_copyCollisionMapShader.uniform1i("collisionImageSrc",1);
    m_copyCollisionMapShader.uniform1i("collisionImageDst",3);
    m_particleRenderShader.uniform1f("renderSize",m_particleRenderSize);
    m_particleRenderShader.uniform1f("renderScale",m_simDomainScale);
    m_particleRenderShader.uniform1f("brightness",m_particleBrightness);
    m_particleRenderShader.uniform3f("defaultColor",m_particleColor);
    m_particleRenderShader.uniform1b("colorcodeVelocity",m_colorcodeVelocity);
    m_particleRenderShader.uniform1b("falloff",m_falloff);
    m_particleRenderShader.uniform2f("domainSize", m_domainSize);
    m_addParticlesShader.uniform1f("spawnedPerCell", m_spawnParticlesPerCell);

    m_g2pShader.uniform1i("gridVX",3);
    m_g2pShader.uniform1i("gridVY",4);
    m_g2pShader.uniform1i("gridM",5);
    m_g2pShader.uniform1i("numParticles",m_numParticles);
    m_g2pShader.uniform2f("simDomain",m_domainSize);
    m_g2pShader.uniform1f("timestep",m_timestep);
    m_g2pShader.uniform1f("pMass",m_particleMass);
    m_g2pShader.uniform1f("gridCellSize",1);
    m_g2pShader.uniform1f("apicFactor",4);

    m_gridUpdateShader.uniform1i("gridVX",3);
    m_gridUpdateShader.uniform1i("gridVY",4);
    m_gridUpdateShader.uniform1i("gridM",5);
    m_gridUpdateShader.uniform1i("collision",1);
    m_gridUpdateShader.uniform2f("simDomain",m_domainSize);
    m_gridUpdateShader.uniform1f("timestep",m_timestep);

    m_p2gShaderAtomic.uniform1i("numParticles",m_numParticles);
    m_p2gShaderAtomic.uniform1f("pMass",m_particleMass);
    m_p2gShaderAtomic.uniform1f("gridCellSize",1);
    m_p2gShaderAtomic.uniform1f("apicFactor",4);
    m_p2gShaderAtomic.uniform1f("bulkModulus",m_bulkModulus);
    m_p2gShaderAtomic.uniform1f("exponentialGamma",m_exponentialGamma);
    m_p2gShaderAtomic.uniform1f("timestep",m_timestep);
    m_p2gShaderAtomic.uniform1i("gridVX",3);
    m_p2gShaderAtomic.uniform1i("gridVY",4);
    m_p2gShaderAtomic.uniform1i("gridM",5);
//    m_p2gShader.uniform2f("simDomain",m_domainSize);
//    m_p2gShader.uniform1f("gridCellSize",1);
//    m_p2gShader.uniform1f("apicFactor",4);
//    m_p2gShader.uniform1f("pMass",m_particleMass);
//    m_p2gShader.uniform1f("timestep",m_timestep);
//    m_p2gShader.uniform1f("bulkModulus",m_bulkModulus);
//    m_p2gShader.uniform1f("exponentialGamma",m_exponentialGamma);

    // set up shader for collision map renderer
    m_collisionMapRenderer.setScreenFillShader(PROJECT_SHADER_PATH"collisionMap.frag");
    m_collisionMapRenderer.shader().uniform1i("collisionMap",2);
    m_collisionMapRenderer.shader().uniform3f("color",m_collisionColor);
    m_collisionMapRenderer.shader().uniform3f("bgcolor",m_backgroundColor);

    // gl options
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void mpmSolver2D::setWindowSize(int width, int height)
{
    m_currentWidth = width;
    m_currentHeight = height;
    m_domainSize = glm::vec2((float(width) / float(height)) * float(m_gridHeight), m_gridHeight);
    m_simDomainScale = float(height)/float(m_gridHeight);

    m_gridVelX = mpu::gph::Texture(mpu::gph::TextureTypes::texture2D, GL_R32F, 1, m_domainSize.x, m_domainSize.y);
    m_gridVelY = mpu::gph::Texture(mpu::gph::TextureTypes::texture2D, GL_R32F, 1, m_domainSize.x, m_domainSize.y);
    m_gridMass = mpu::gph::Texture(mpu::gph::TextureTypes::texture2D, GL_R32F, 1, m_domainSize.x, m_domainSize.y);
    mpu::gph::Texture newCollisionMap(mpu::gph::TextureTypes::texture2D, GL_R16F, 1, m_domainSize.x, m_domainSize.y);

    // copy old collision map
    newCollisionMap.bindImage(3,GL_WRITE_ONLY, GL_R16F);
    m_copyCollisionMapShader.dispatch(m_domainSize/2, {16, 16});
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    m_gridCollision = std::move(newCollisionMap);

    // rebind all the images
    m_gridVelX.bindImage(3,GL_READ_WRITE, GL_R32F);
    m_gridVelY.bindImage(4,GL_READ_WRITE, GL_R32F);
    m_gridMass.bindImage(5,GL_READ_WRITE, GL_R32F);
    m_gridCollision.bindImage(1,GL_READ_WRITE, GL_R16F);

    // rebind collision map as texture
    m_gridCollision.bind(2);

    // uniforms
    m_particleRenderShader.uniform1f("renderScale",m_simDomainScale);
    m_particleRenderShader.uniform2f("domainSize", m_domainSize);
    m_g2pShader.uniform2f("simDomain",m_domainSize);
    m_gridUpdateShader.uniform2f("simDomain",m_domainSize);

//    m_p2gShader.uniform2f("simDomain",m_domainSize);
}

void mpmSolver2D::applyExternalAcc(glm::vec2 force)
{
    m_additionalAcc += force;
}

void mpmSolver2D::addParticles(glm::vec2 position)
{
    position.x *= m_domainSize.x;
    position.y *= m_domainSize.y;
    int numSqrt = int(2.0f * m_particleBrushSize * m_spawnParticlesPerCell + 1);
    int numAdded = numSqrt*numSqrt;

    // check if there is still space
    if(m_particleBufferCapacity <= m_numParticles+numAdded)
    {
        // allocate new buffers
        int newCapacity = m_particleBufferCapacity + glm::max(m_particleBufferResizeValue, numAdded);
        mpu::gph::Buffer<glm::vec2> newParticlePositon(newCapacity);
        mpu::gph::Buffer<glm::vec2> newParticleVelocity(newCapacity);
        mpu::gph::Buffer<glm::mat2> newParticleF(newCapacity);
        mpu::gph::Buffer<glm::mat2> newParticleAffine(newCapacity);
        mpu::gph::Buffer<float> newInitialVolume(newCapacity);

        // copy data
        newParticlePositon.copy(m_particlePositon,m_particleBufferCapacity);
        newParticleVelocity.copy(m_particleVelocity,m_particleBufferCapacity);
        newParticleF.copy(m_particleF,m_particleBufferCapacity);
        newParticleAffine.copy(m_particleAffine,m_particleBufferCapacity);
        newInitialVolume.copy(m_initialVolume,m_particleBufferCapacity);

        // move buffers
        m_particlePositon = std::move(newParticlePositon);
        m_particleVelocity = std::move(newParticleVelocity);
        m_particleF = std::move(newParticleF);
        m_particleAffine = std::move(newParticleAffine);
        m_initialVolume = std::move(newInitialVolume);
        m_particleBufferCapacity = newCapacity;

        // bind all the buffers
        m_particlePositon.bindBase(2,GL_SHADER_STORAGE_BUFFER);
        m_particleVelocity.bindBase(3,GL_SHADER_STORAGE_BUFFER);
        m_particleF.bindBase(4,GL_SHADER_STORAGE_BUFFER);
        m_particleAffine.bindBase(5,GL_SHADER_STORAGE_BUFFER);
        m_initialVolume.bindBase(6,GL_SHADER_STORAGE_BUFFER);

        m_vao.addAttributeBufferArray(0,0,m_particlePositon,0, sizeof(glm::vec2),2,0);
        m_vao.addAttributeBufferArray(1,1,m_particleVelocity,0, sizeof(glm::vec2),2,0);
    }

    // generate new particles
    m_addParticlesShader.uniform2f("addPosition", position);
    m_addParticlesShader.uniform1f("addRadius", m_particleBrushSize);
    m_addParticlesShader.uniform1i("startID", m_numParticles);
    m_addParticlesShader.dispatch(numAdded/2,32);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    m_numParticles += numAdded;

    // update uniforms
    m_g2pShader.uniform1i("numParticles",m_numParticles);
    m_p2gShaderAtomic.uniform1i("numParticles",m_numParticles);
}

void mpmSolver2D::addCollisionObject(glm::vec2 position)
{
    position.x *= m_domainSize.x;
    position.y *= m_domainSize.y;
    m_addCollisionObjectShader.uniform2i("addPosition", position);
    m_addCollisionObjectShader.uniform1f("addRadius", m_obstacleBrushSize);

    m_addCollisionObjectShader.dispatch(m_domainSize/2, {16, 16});
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void mpmSolver2D::drawUI(bool* shouldBeDrawn)
{
    if(ImGui::Begin("mpmSolver",shouldBeDrawn))
    {
        ImGui::Text("NumParticles: %i", m_numParticles);
        if(ImGui::DragFloat("Timestep",&m_timestep,0.001))
        {
            m_g2pShader.uniform1f("timestep",m_timestep);
            m_p2gShaderAtomic.uniform1f("timestep",m_timestep);
            m_gridUpdateShader.uniform1f("timestep",m_timestep);
        }

        ImGui::DragInt("Timesteps per Frame",&m_timestepsPerFrame);

        if(ImGui::DragInt("Grid Resolution", &m_gridHeight))
            setWindowSize(m_currentWidth, m_currentHeight); // trigger grid rebuild

        ImGui::Separator();

        ImGui::DragFloat("Collision Brush Size",&m_obstacleBrushSize,0.5);
        ImGui::DragFloat("Particle Brush Size",&m_particleBrushSize,0.5);
        if(ImGui::DragFloat("Particles spawned per cell", &m_spawnParticlesPerCell, 0.02))
            m_addParticlesShader.uniform1f("spawnedPerCell", m_spawnParticlesPerCell);

        if(ImGui::Button("Remove Particles")) {
            m_numParticles = 0;
            m_g2pShader.uniform1i("numParticles",m_numParticles);
            m_p2gShaderAtomic.uniform1i("numParticles",m_numParticles);
        }
        ImGui::SameLine();
        if(ImGui::Button("Remove Obstacles"))
            clearCollisionMap();

        ImGui::Separator();

        ImGui::DragFloat2("Gravity",glm::value_ptr(m_gravity));
        if(ImGui::DragFloat("ParticleMass", &m_particleMass))
        {
            m_g2pShader.uniform1f("pMass",m_particleMass);
            m_p2gShaderAtomic.uniform1f("pMass",m_particleMass);
        }
    }
    ImGui::End();
}

void mpmSolver2D::advanceSimulation()
{
    for(int i =0; i<m_timestepsPerFrame; i++)
    {
        m_gridUpdateShader.uniform2f("externalAcc", m_gravity + m_additionalAcc);
        m_additionalAcc = glm::vec2(0);

        float clear = 0.0f;
        m_gridVelX.clear(&clear,GL_RED,GL_FLOAT,0);
        m_gridVelY.clear(&clear,GL_RED,GL_FLOAT,0);
        m_gridMass.clear(&clear,GL_RED,GL_FLOAT,0);

        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // use rasterization pipeline for particle to grid
//        m_fbo.attach( GL_COLOR_ATTACHMENT0, m_gridVelocityMass);
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_ONE, GL_ONE);
//        m_fbo.use();
//        glViewport(0,0,m_domainSize.x,m_domainSize.y);
//        m_vao.bind();
//        m_p2gShader.use();
//        glDrawArrays(GL_POINTS,0,m_numParticles);
//        m_fbo.disable();
//        glDisable(GL_BLEND);

        m_p2gShaderAtomic.dispatch(m_numParticles,m_g2pGroupSize);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        m_gridUpdateShader.dispatch(m_domainSize,m_gridUpdateGroupSize);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        m_g2pShader.dispatch(m_numParticles,m_g2pGroupSize);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }
}

void mpmSolver2D::drawCollisionMap()
{
//    m_collisionMapRenderer.draw();
    static mpu::gph::ScreenFillingTri tri;
    tri.setScreenFillShader(MPU_LIB_SHADER_PATH"drawTexture.frag");
    tri.shader().uniform1i("colorMap",4);
//    m_gridVelX.bind(4);
//    m_gridVelY.bind(4);
    m_gridMass.bind(4);

    tri.draw();
}

void mpmSolver2D::drawParticles()
{
    m_vao.bind();
    m_particleRenderShader.use();
    glDrawArrays(GL_POINTS,0,m_numParticles);
}

void mpmSolver2D::clearCollisionMap()
{
    float clear = 0.5;
    m_gridCollision.clear(&clear,GL_RED,GL_FLOAT,0);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
