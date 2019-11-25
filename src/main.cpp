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

    // create mpm solver
    mpmSolver2D solver(float(width) / float(height));

    // add resize callback
    window.addFBSizeCallback([&](int w, int h)
                             {
                                 glViewport(0,0,w,h);
                                 width = w;
                                 height = h;
                                 solver.setAspectRation(float(width) / float(height));
                             });

    float backgroundBrushSize = 50;
    float particleBrushSize = 50;
    bool simCanRun = true; //!< should the simulation be running?
    bool simPaused = false; //!< should the simulation be running?
    bool showSolverUI = true; //!< should solver UI be drawn?
    bool drawGeneralUI = true; //!< draw the general UI window?
    bool vsync = true; //!< is vsync on?
    mpu::gph::enableVsync(vsync);

    // add some keybindings
    mpu::gph::Input::addButton("ToggleSolverUI","Toggles visibility of the solver ui", [&](mpu::gph::Window& wnd){ showSolverUI = !showSolverUI;});
    mpu::gph::Input::mapKeyToInput("ToggleSolverUI", GLFW_KEY_S);
    mpu::gph::Input::addButton("TogglePause","Toggles simulation pause state", [&](mpu::gph::Window& wnd){ simPaused = !simPaused;});
    mpu::gph::Input::mapKeyToInput("TogglePause", GLFW_KEY_P);
    mpu::gph::Input::addButton("ToggleGeneralUI","Toggles visibility of the general ui", [&](mpu::gph::Window& wnd){ drawGeneralUI = !drawGeneralUI;});
    mpu::gph::Input::mapKeyToInput("ToggleGeneralUI", GLFW_KEY_G);

    // Main loop
    while (window.frameEnd(), mpu::gph::Input::update(), window.frameBegin())
    {
        // show imgui demo window
//        ImGui::ShowDemoWindow();

        if(drawGeneralUI)
        {
            if(ImGui::Begin("mpmDemo",&drawGeneralUI))
            {
                ImGui::Text("Frametime: %f", mpu::gph::Input::deltaTime());
                ImGui::Text("FPS: %f", 1.0f / mpu::gph::Input::deltaTime());

                if(ImGui::Checkbox("V-Sync",&vsync))
                    mpu::gph::enableVsync(vsync);

                if(ImGui::Button((simPaused)? "Continue Simulation" : "Pause Simulation"))
                    simPaused = !simPaused;

                if(ImGui::Button("Reset Simulation"))
                {
                    solver = mpmSolver2D(float(width) / float(height));
                }

                ImGui::DragFloat("Collision Brush Size",&backgroundBrushSize);
                ImGui::DragFloat("Particle Brush Size",&backgroundBrushSize);
            }
            ImGui::End();
        }

        if(showSolverUI)
            solver.drawUI(&showSolverUI);

        if(simCanRun && !simPaused)
            solver.advanceSimulation();

        solver.drawParticles();
        solver.drawParticles();

    }
    return 0;
}