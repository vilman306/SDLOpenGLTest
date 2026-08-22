#define SDL_MAIN_USE_CALLBACKS 1

#include "Game.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <stdexcept>
#include <iostream>
#include "Config.h"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    Game* game = new Game();
    *appstate = game;

    try {
        SDL_SetAppMetadata(Config::WindowTitle, NULL, NULL);
        if (!game->GetScene().IsValid())
            throw std::runtime_error("Failure when initializing Scene.");
    }
    catch (const std::exception& e) {

        std::cout << "ERROR: " << e.what() << std::endl;
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    Game* game = static_cast<Game*>(appstate);
    delete game;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }

    Game* game = static_cast<Game*>(appstate);

    try {
        game->ProcessInput(event);
    }
    catch (std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    Game* game = static_cast<Game*>(appstate);
    try {
        game->Update();
    }
    catch (std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}
