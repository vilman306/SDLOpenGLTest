#ifndef GAME_HPP
#define GAME_HPP

#include "Scene.hpp"

class Game
{
    public:
        Game();
        virtual ~Game();

        void Update();
        Scene& GetScene() { return m_Scene; }
        void ProcessInputEvent(SDL_Event* event);

    protected:

    private:
        Scene m_Scene;
        Camera m_Camera; // Should be an entity with a CameraComponent, stored in World

};

#endif // GAME_HPP
