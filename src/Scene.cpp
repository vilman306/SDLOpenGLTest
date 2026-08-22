#include "Scene.hpp"
#include "Config.h"

#include <glad/gl.h>

#include <SDL3/SDL_init.h>
#include <stdexcept>
#include <iostream>
#include <SDL3/SDL_timer.h>

#include "Sprite.hpp"

void SetupOpenGL(); // definition further down
void SetupSprites();

Scene::Scene()
{
//    SDL_Window* window = nullptr;
//    SDL_Renderer* renderer = nullptr;
//    try {
//        if (!SDL_Init(SDL_INIT_VIDEO))
//            throw std::runtime_error("Failure on SDL_Init: " + std::string(SDL_GetError()));
//        if (!SDL_CreateWindowAndRenderer(Config::WindowTitle, Config::WindowWidth, Config::WindowHeight, SDL_WINDOW_RESIZABLE, &window, &renderer))
//            throw std::runtime_error("Failure on SDL_CreateWindowAndRenderer: " + std::string(SDL_GetError()));
//        if (!SDL_SetRenderLogicalPresentation(renderer, Config::WindowWidth, Config::WindowHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX))
//            throw std::runtime_error("Failure on SDL_SetRenderLogicalPresentation: " + std::string(SDL_GetError()));
//    }
//
//    catch (const std::exception& e)
//    {
//        std::cout << "ERROR: " << e.what() << std::endl;
//        m_IsValid = false;
//    }
//
//    m_Window.reset(window);
//    m_Renderer.reset(renderer);
//    sprite.LoadTexture("default.png", m_Renderer.get());
    SDL_Window* window = nullptr;
//    SDL_Renderer* renderer = nullptr;

    try {
        if (!SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error("Failure on SDL_Init: " + std::string(SDL_GetError()));

        if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1))
            throw std::runtime_error(SDL_GetError());
        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3))
            throw std::runtime_error(SDL_GetError());
        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3))
            throw std::runtime_error(SDL_GetError());
        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))
            throw std::runtime_error(SDL_GetError());

//        if (!SDL_CreateWindowAndRenderer(Config::WindowTitle, Config::WindowWidth, Config::WindowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL, &window, &renderer))
//            throw std::runtime_error("Failure on SDL_CreateWindowAndRenderer: " + std::string(SDL_GetError()));
        if (!(window = SDL_CreateWindow(Config::WindowTitle, Config::WindowWidth, Config::WindowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)))
            throw std::runtime_error("Failure on SDL_CreateWindow: " + std::string(SDL_GetError()));

        SDL_GLContext glContext;
        if (!(glContext = SDL_GL_CreateContext(window)))
            throw std::runtime_error("Failure on SDL_GL_CreateContext: " + std::string(SDL_GetError()));

        // window and glContext OK, store/own them.
        m_Window.reset(window);
        m_GLContext.reset(glContext);

        int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress); // Initializes GLAD?
        if (version == 0)
            throw std::runtime_error("Failed to initialize GLAD");
        std::cout << "GL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

        SetupOpenGL();
        SetupSprites();
    }

    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        m_IsValid = false;
    }
}

Scene::~Scene()
{
    //dtor
}


#include <fstream>
#include <sstream>
std::string LoadShaderSource(const std::string& filePath)
{
    // see https://stackoverflow.com/questions/2141749/what-does-ifstreamrdbuf-actually-do
    std::ifstream file(filePath);
    if (file.fail())
        throw std::runtime_error(std::string("Failed to open file at '") + filePath + "'.");
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint CreateShader(GLenum shaderType, const std::string& sourcePath)
{
    GLuint shader = glCreateShader(shaderType);
    const GLchar* vertexSource = LoadShaderSource("default.vert").c_str();
    glShaderSource(shader, 1, &vertexSource, nullptr);
    glCompileShader(shader);
    // Check for compilation errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Failed to compile vertex shader: ") + infoLog);
    }
    return shader;
}

GLuint CreateShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Failed to link shader program: ") + infoLog);
    }
}

GLuint VBO;
glm::mat4 viewMatrix(1.0f);
glm::mat4 projectionMatrix(1.0f);

void SetupOpenGL()
{
    glClearColor(0.25f, 0.25f, 0.4f, 1.0f); // Set background color

    glGenBuffers(1, &VBO); // Create vertex buffer object (for sprites)

    // Create shaders
    GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, "default.vert");
    GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, "default.frag");

    // Create and activate shader program
    GLuint shaderProgram = CreateShaderProgram(vertexShader, fragmentShader);
    glUseProgram(shaderProgram);

    // Delete shaders now that they're linked in the shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create matrices
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -3.0f)); // For "camera"
    projectionMatrix = glm::perspective(glm::radians(45.0f), static_cast<float>(Config::WindowWidth) / Config::WindowHeight, 0.1f, 100.0f); // For clipping and depth

    GLint modelMatrixLocation = glGetUniformLocation()
}

Sprite sprite1(VBO);
Sprite sprite2(VBO);
Sprite sprite3(VBO);

void SetupSprites()
{
    sprite1.m_Transform.Position = {40.0f, 40.0f, -2.0f};
    sprite2.m_Transform.Position = {120.0f, 120.0f, -1.0f};
    sprite3.m_Transform.Position = {80.0f, 80.0f, 0.0f};

    sprite1.UpdateVBO();
    sprite2.UpdateVBO();
    sprite3.UpdateVBO();
}




void PrepareToDrawSprite(Sprite& p_Sprite)
{

}

void DrawSprite(Sprite& p_Sprite)
{
    // Render everythin in VBO
}



void Scene::Update(double deltaTime)
{
    try
    {
        // RENDERING:
        // --------
        // Clear
//        SDL_SetRenderDrawColorFloat(m_Renderer.get(), 0.25f, 0.25f, 0.25f, SDL_ALPHA_OPAQUE_FLOAT);
//        SDL_RenderClear(m_Renderer.get());

        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(m_Window.get());
//
//        DrawSprite(sprite1);
//        DrawSprite(sprite2);
//        DrawSprite(sprite3);

//        sprite.Draw(m_Renderer.get());

        // Draw to screen
//        SDL_RenderPresent(m_Renderer.get());
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
}


void Scene::ProcessInput(SDL_Event* event)
{
    // New window resolution
    if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
    {
        glViewport(0, 0, event->window.data1, event->window.data2);
    }
}
