#ifndef SCENE_HPP
#define SCENE_HPP

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <memory>

struct WindowDeleter
{
    void operator()(SDL_Window* window) const {
        SDL_DestroyWindow(window);
    }
};

struct RendererDeleter
{
    void operator()(SDL_Renderer* renderer) const {
        SDL_DestroyRenderer(renderer);
    }
};

struct GLContextDeleter
{
    void operator()(SDL_GLContextState* glContext) const {
        SDL_GL_DestroyContext(glContext);
    }
};

class Scene
{
    public:
        Scene();
        virtual ~Scene();

        void Update(double deltaTime);
        bool IsValid() const { return m_IsValid; }
        void ProcessInput(SDL_Event* event);

    protected:

    private:
        std::unique_ptr<SDL_Window, WindowDeleter> m_Window;
//        std::unique_ptr<SDL_Renderer, RendererDeleter> m_Renderer;
        std::unique_ptr<SDL_GLContextState, GLContextDeleter> m_GLContext; // SDL_GLContextState == std::remove_pointer_t<SDL_GLContext>
        bool m_IsValid = true;
};

#endif // SCENE_HPP
