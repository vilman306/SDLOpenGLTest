#pragma once

#include <string>
#include <SDL3/SDL_filesystem.h>

namespace Config {
    const int WindowWidth = 500;
    const int WindowHeight = 400;
    const float AspectRatio = static_cast<float>(WindowWidth) / WindowHeight;
    inline const char* WindowTitle = "Good Game";

    inline std::string ShaderDirectory = std::string(SDL_GetBasePath()) + "../../src/";
}
