#ifndef INPUTSYSTEM_HPP
#define INPUTSYSTEM_HPP

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>

struct CameraInputState
{
    float MoveForward = 0.0f; // -1.0f < 1.0f
    float MoveRight = 0.0f;   // -1.0f < 1.0f
    float LookDeltaX = 0.0f;  // Delta/relative mouse movement in x direction
    float LookDeltaY = 0.0f;  // Delta/relative mouse movement in y direction
};

class InputSystem
{
public:
    InputSystem();
    virtual ~InputSystem();

    void Poll()
    {
        m_KeyStates = SDL_GetKeyboardState(nullptr);
        SDL_GetRelativeMouseState(&m_MouseRelX, &m_MouseRelY);
        m_MouseRelY *= -1.0f;
    }
    CameraInputState GetCameraInput()
    {
        CameraInputState cameraInput;
        if (m_KeyStates[SDL_SCANCODE_W])
            cameraInput.MoveForward += 1.0f;
        if (m_KeyStates[SDL_SCANCODE_S])
            cameraInput.MoveForward -= 1.0f;
        if (m_KeyStates[SDL_SCANCODE_A])
            cameraInput.MoveRight -= 1.0f;
        if (m_KeyStates[SDL_SCANCODE_D])
            cameraInput.MoveRight += 1.0f;
        cameraInput.LookDeltaX = m_MouseRelX;
        cameraInput.LookDeltaY = m_MouseRelY;
        return cameraInput;
    }

protected:

private:
    const bool* m_KeyStates;
    float m_MouseRelX;
    float m_MouseRelY;
};

#endif // INPUTSYSTEM_HPP
