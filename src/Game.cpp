#include "Game.hpp"
#include <SDL3/SDL_timer.h>
#include <stdexcept>

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
    double currentTime = static_cast<double>(SDL_GetPerformanceCounter()) / SDL_GetPerformanceCounter();
    static double previousTime = currentTime;

    double deltaTime = currentTime - previousTime;

    if (!m_Scene.IsValid())
        throw std::runtime_error("Scene not valid at start of Game::Update()");
    m_Scene.Update(deltaTime);

    previousTime = currentTime;
}

void Game::ProcessInput(SDL_Event* event)
{
    m_Scene.ProcessInput(event);
}
