//#define SDL_MAIN_USE_CALLBACKS 1 /* Use callbacks instead of main */
//#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h>
//#include <stdexcept>
//#include <string>
//#include <iostream>
//#include <vector>
//
//SDL_Window* window = nullptr;
//SDL_Renderer* renderer = nullptr;
//std::vector<SDL_FPoint> points;
//
//SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
//{
//    try
//    {
//        SDL_SetAppMetadata("Test Game", "0.0.1", "se.ville.game");
//        if (!SDL_Init(SDL_INIT_VIDEO))
//            throw std::runtime_error("Failure on SDL_Init: " + std::string(SDL_GetError()));
//        if (!SDL_CreateWindowAndRenderer("Test Game", 500, 400, SDL_WINDOW_RESIZABLE, &window, &renderer))
//            throw std::runtime_error("Failure on SDL_CreateWindowAndRenderer: " + std::string(SDL_GetError()));
//        if (!SDL_SetRenderLogicalPresentation(renderer, 500, 400, SDL_LOGICAL_PRESENTATION_LETTERBOX))
//            throw std::runtime_error("Failure on SDL_SetRenderLogicalPresentation: " + std::string(SDL_GetError()));
//
//        return SDL_APP_CONTINUE;
//    }
//
//    catch (const std::exception& e)
//    {
//        std::cout << "ERROR: " << e.what() << std::endl;
//        return SDL_APP_FAILURE;
//    }
//}
//
//SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
//{
//    if (event->type == SDL_EVENT_QUIT)
//    {
//        return SDL_APP_SUCCESS;
//    }
//
//    return SDL_APP_CONTINUE;
//}
//
//SDL_AppResult SDL_AppIterate(void* appstate)
//{
//    try
//    {
//        const double time = static_cast<double>(SDL_GetTicks()) / 1000.0; // in seconds
//
//        // RENDERING:
//        // Clear
//        SDL_SetRenderDrawColorFloat(renderer, 0.25f, 0.25f, 0.25f, SDL_ALPHA_OPAQUE_FLOAT);
//        SDL_RenderClear(renderer);
//        // Create and draw points
////        points.clear();
//        for (int i = 0; i < 100000; i++)
//        {
////            points.push_back( { SDL_randf() * 200.0f, SDL_randf() * 200.0f } );
//            float padding = 50.0f;
//            float width = 500.0f - padding;
//            float height = 400.0f - padding;
//            float randX = SDL_randf();
//            float randY = SDL_randf();
//            SDL_FPoint point = {randX * width + padding/2.0f, randY * height + padding/2.0f};
//            float offLimit = 0.3f;
//            float offScale = 1.5f;
//            float offColor = SDL_clamp((SDL_randf() - 0.5f) * offScale, -offLimit, offLimit);
//            float r = SDL_clamp(randX + offColor, 0.0f, 1.0f);
//            float g = SDL_clamp(1.0f - randY + offColor, 0.0f, 1.0f);
//            float b = SDL_clamp(randY + offColor, 0.0f, 1.0f);
//            SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE_FLOAT);
//            SDL_RenderPoint(renderer, point.x, point.y);
//        }
//
////        SDL_RenderPoints(renderer, points.data(), points.size());
//        // Draw to screen
//        SDL_RenderPresent(renderer);
//
//        return SDL_APP_CONTINUE;
//    }
//    catch (const std::exception& e)
//    {
//        std::cout << "ERROR: " << e.what() << std::endl;
//        return SDL_APP_FAILURE;
//    }
//}
//
//void SDL_AppQuit(void* appstate, SDL_AppResult result)
//{
//
//}
