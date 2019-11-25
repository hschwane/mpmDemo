/*
 * Project
 * main.cpp
 *
 * @author: Hendrik Schwanekamp
 * @mail:   hendrik.schwanekamp@gmx.net
 *
 * Copyright (c) 2019 Hendrik Schwanekamp
 *
 */
#include <iostream>
#include <mpUtils/mpUtils.h>
#include <mpUtils/mpGraphics.h>
#include "mpmSolver2D.h"

int main()
{
    // setup logging
    mpu::Log myLog( mpu::LogLvl::ALL, mpu::ConsoleSink());
#if defined(NDEBUG)
    myLog.printHeader("mpmDemo", "", "", "Release");
#else
    myLog.printHeader("mpmDemo", "", "", "Debug");
#endif

    // add shader include pathes
    mpu::gph::addShaderIncludePath(MPU_LIB_SHADER_PATH"include");
    mpu::gph::addShaderIncludePath(PROJECT_SHADER_PATH"include");

    // create window and gui
    int width = 600;
    int height = 600;
    mpu::gph::Window window(width, height,"EZR volume rendering");
    mpu::gph::enableVsync(true);
    ImGui::create(window);

    // set background color
    glClearColor( .2f, .2f, .2f, 1.0f);

    // add resize callback
    window.addFBSizeCallback([&](int w, int h)
                             {
                                 glViewport(0,0,w,h);
                                 width = w;
                                 height = h;
                             });

    // Main loop
    while (window.frameEnd(), mpu::gph::Input::update(), window.frameBegin())
    {
        // show imgui demo window
        ImGui::ShowDemoWindow();
    }
    return 0;
}