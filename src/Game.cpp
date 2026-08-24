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

    // Camear movement (TODO: MOVE ELSEWHERE)
    const bool* key_states = SDL_GetKeyboardState(nullptr);
    glm::vec2 cameraXZMovement(0.0f); // x = X (right), y = Z (backward)
    if (key_states[SDL_SCANCODE_W])
        cameraXZMovement.y -= 1.0f;
    if (key_states[SDL_SCANCODE_S])
        cameraXZMovement.y += 1.0f;
    if (key_states[SDL_SCANCODE_A])
        cameraXZMovement.x -= 1.0f;
    if (key_states[SDL_SCANCODE_D])
        cameraXZMovement.x += 1.0f;
    glm::vec3 movementAxis = glm::vec3(cameraXZMovement.x, 0, cameraXZMovement.y);
    float movementSpeed = 200.0f;

    if (glm::length(movementAxis) > 0.0f) {
        m_Camera.Translate(movementSpeed * deltaTime, movementAxis);

    }

    // Update scene
    if (!m_Scene.IsValid())
        throw std::runtime_error("Scene not valid at start of Game::Update()");
    m_Scene.Update(deltaTime, m_Camera);

    previousTime = currentTime;
}

void Game::ProcessInputEvent(SDL_Event* event)
{
    m_Scene.ProcessInputEvent(event);
}
