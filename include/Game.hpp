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
        void ProcessInput(SDL_Event* event);

    protected:

    private:
        Scene m_Scene;
};

#endif // GAME_HPP
