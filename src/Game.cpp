#include "Game.hpp"
#include <SDL3/SDL_timer.h>
#include <stdexcept>
#include <iostream>


Game::Game()
{
    //ctor
}

Game::~Game()
{
    //dtor
}

void Game::Update()
{
    // Calculate delta time
    double currentTime = static_cast<double>(SDL_GetPerformanceCounter()) / SDL_GetPerformanceFrequency();
    static double previousTime = currentTime;
    double deltaTime = currentTime - previousTime;

    m_InputSystem.Poll(); // Polls/gets input
    m_Camera.Update(m_InputSystem.GetCameraInput(), deltaTime);


    // Camear movement (TODO: MOVE ELSEWHERE)


//    m_Camera.Move(cameraXZMovement, deltaTime);
//
//    float xRel, yRel = 0.0f;
//    SDL_GetRelativeMouseState(&xRel, &yRel);
//    if (xRel != 0.0f || yRel != 0.0f)
//        std::cout << xRel << "   " << yRel << "\n";


    // Update scene
    if (!m_Scene.IsValid())
        throw std::runtime_error("Scene not valid at start of Game::Update()");
    m_Scene.Update(deltaTime, m_Camera);

    previousTime = currentTime;
}

void Game::ProcessInputEvent(SDL_Event* event)
{
    m_Scene.ProcessInputEvent(event);

//    if (event->type == SDL_EVENT_MOUSE_MOTION) {
//        float xrel = event->motion.xrel;
//        float yrel = -event->motion.yrel;
//        m_Camera.Look(xrel, yrel);
//    }
}
