#include "Scene.hpp"
#include "Config.h"

#include <glad/gl.h>

#include <SDL3/SDL_init.h>
#include <stdexcept>
#include <iostream>
#include <SDL3/SDL_timer.h>

#include "Sprite.hpp"

void SetupOpenGL(); // definition further down

Scene::Scene()
{
    SDL_Window* window = nullptr;

    try {
        if (!SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error("Failure on SDL_Init: " + std::string(SDL_GetError()));

        if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1))
            throw std::runtime_error(SDL_GetError());
        if (!SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24))
            throw std::runtime_error(SDL_GetError());
        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3))
            throw std::runtime_error(SDL_GetError());
        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3))
            throw std::runtime_error(SDL_GetError());
        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))
            throw std::runtime_error(SDL_GetError());

        if (!(window = SDL_CreateWindow(Config::WindowTitle, Config::WindowWidth, Config::WindowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)))
            throw std::runtime_error("Failure on SDL_CreateWindow: " + std::string(SDL_GetError()));

        SDL_GLContext glContext;
        if (!(glContext = SDL_GL_CreateContext(window)))
            throw std::runtime_error("Failure on SDL_GL_CreateContext: " + std::string(SDL_GetError()));

        if (!SDL_SetWindowRelativeMouseMode(window, true))
            throw std::runtime_error("Failure on SDL_SetWindowRelativeMouseMode: " + std::string(SDL_GetError()));

        if (!SDL_GL_SetSwapInterval(-1) && !SDL_GL_SetSwapInterval(1)) // Try turning on adaptive vsync, otherwise turn on normal vsync
            throw std::runtime_error(SDL_GetError());

        // window and glContext OK, store/own them.
        m_Window.reset(window);
        m_GLContext.reset(glContext);

        int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress); // Initializes GLAD?
        if (version == 0)
            throw std::runtime_error("Failed to initialize GLAD");
        std::cout << "GL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

        glClearColor(0.25f, 0.25f, 0.4f, 1.0f); // Set background color

        // Create Shader (program) for drawing Sprites
        Shader spriteShader("default.vert", "default.frag");
        m_Shaders.emplace(ShaderType::Sprite, std::move(spriteShader));

        // TODO: Move to Material class:
        // -----------------------
        glEnable(GL_DEPTH_TEST); // Enable depth testing
        glEnable(GL_BLEND); // Enable blending, so can display transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set source and destination factors, see https://learnopengl.com/Advanced-OpenGL/Blending
        // -----------------------

        SetupSprites();
    }

    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        m_IsValid = false;
    }
}

void Scene::CreateBox(const std::string& texturePath, glm::vec3 centerPosition)
{
    Sprite back(texturePath, glm::vec3(0.0f, 0.0f, -1.0f));
    float width = back.width * Sprite::sizeScaleFactor;
    float height = back.height * Sprite::sizeScaleFactor;
    back.SetPosition(centerPosition + glm::vec3(0.0f, 0.0f, -0.5f * height));
    Sprite bottom(texturePath, glm::vec3(0.0f, -1.0f, 0.0f));
    bottom.SetPosition(centerPosition + glm::vec3(0.0f, -0.5f * height, 0.0f));
    Sprite top(texturePath, glm::vec3(0.0f, 1.0f, 0.0f));
    top.SetPosition(centerPosition + glm::vec3(0.0f, 0.5f * height, 0.0f));
    Sprite left(texturePath, glm::vec3(-1.0f, 0.0f, 0.0f));
    left.SetPosition(centerPosition + glm::vec3(-0.5f * width, 0.0f, 0.0f));
    Sprite right(texturePath, glm::vec3(1.0f, 0.0f, 0.0f));
    right.SetPosition(centerPosition + glm::vec3(0.5f * width, 0.0f, 0.0f));
    Sprite front(texturePath, glm::vec3(0.0f, 0.0f, 1.0f));
    front.SetPosition(centerPosition + glm::vec3(0.0f, 0.0f, 0.5f * height));
    std::array<Sprite, 6> box = {std::move(back), std::move(bottom), std::move(top), std::move(left), std::move(right), std::move(front)};
    for (Sprite& boxSprite : box) {
        m_Sprites.push_back(std::move(boxSprite));
    }
}

void Scene::SetupSprites()
{
    CreateBox("alphatest.png", glm::vec3(0.0f, 0.0f, -50.0f));
    CreateBox("cubeside.png", glm::vec3(-50.0f, 0.0f, -50.0f));
    CreateBox("default.png", glm::vec3(50.0f, 0.0f, -50.0f));
    Sprite ground("ground.png", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -50.0f, 0.0f), glm::vec3(40.0f, 40.0f, 1.0f));
    m_Sprites.push_back(std::move(ground));
//    CreateBox("lightsource.png", glm::vec3(0.0f, 30.0f, 25.0f));


}

#include <SDL3/SDL_time.h>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
void Scene::DrawSprites(Camera& camera)
{
    // Begin drawing sprites
    Shader* shader = &m_Shaders.at(ShaderType::Sprite);
    shader->Use();
    glBindVertexArray(Sprite::GetMesh().VAO);

    glActiveTexture(GL_TEXTURE0); // Set which texture unit to bind textures to

    // Send camera matrices to shader program's uniforms
    shader->SetUniform<glm::mat4>("view", camera.GetViewMatrix());
    shader->SetUniform<glm::mat4>("projection", camera.GetProjectionMatrix());

    // Sort sprites based on distance (needed for drawing transparent objects correclty: https://learnopengl.com/Advanced-OpenGL/Blending)
    std::vector<Sprite*> spritesSortedByDistance;
    for (Sprite& sprite : m_Sprites)
        spritesSortedByDistance.push_back(&sprite);

    std::sort( spritesSortedByDistance.begin(), spritesSortedByDistance.end(),
            [&camera](const Sprite* a, const Sprite* b) {
                float aToCameraDist = glm::length(camera.GetPosition() - a->GetPosition());
                float bToCameraDist = glm::length(camera.GetPosition() - b->GetPosition());
                return aToCameraDist > bToCameraDist; // Sort in descending order (want to draw objects furthest away first)
            } );

    for (Sprite* sprite : spritesSortedByDistance) {
        shader->SetUniform<glm::mat4>("model", sprite->GetModelMatrix());
        glBindTexture(GL_TEXTURE_2D, sprite->m_Texture.GetID());
        glDrawElements(GL_TRIANGLES, Sprite::GetMesh().nrOfVertexIndices, GL_UNSIGNED_INT, 0);
    }
}

void Scene::Update(double deltaTime, Camera& camera)
{
    try
    {
        // RENDERING:

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawSprites(camera);

        SDL_GL_SwapWindow(m_Window.get());

    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
}


void Scene::ProcessInputEvent(SDL_Event* event)
{
    // New window resolution
    if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
    {
        glViewport(0, 0, event->window.data1, event->window.data2);
    }
}


Scene::~Scene()
{
    // This deletes the Sprite class's static Mesh's buffers and VAO.
    // Must be done here and not later, because "OpenGL closes" after Scene's destruction (Scene owns window) which
    // makes calling glDelete...() crash the program.
    Sprite::GetMesh().Reset();
}
