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


std::string ShaderTypeToString(GLenum type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
        default: return "shader type not defined in function ShaderTypeToString(GLenum type)";
    }
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
    std::string sourceString = LoadShaderSource(sourcePath);
    const GLchar* source = sourceString.c_str();
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    // Check for compilation errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Failed to compile shader of type ") + ShaderTypeToString(shaderType) + " :" + infoLog);
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
    return shaderProgram;
}


glm::mat4 viewMatrix(1.0f);
glm::mat4 projectionMatrix(1.0f);
GLuint shaderProgram; // Shader program for drawing sprites
GLint modelMatrixLocation;
GLint viewMatrixLocation;
GLint projectionMatrixLocation;
GLint spriteTextureLocation;

#include <SDL3/SDL_filesystem.h>
#include <glm/gtc/type_ptr.hpp>
void SetupOpenGL()
{
    glClearColor(0.25f, 0.25f, 0.4f, 1.0f); // Set background color

    // Create shaders

    GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, std::string(SDL_GetBasePath()) + "../../src/default.vert");
    GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, std::string(SDL_GetBasePath()) + "../../src/default.frag");

    // Create and activate shader program
    shaderProgram = CreateShaderProgram(vertexShader, fragmentShader);
    glUseProgram(shaderProgram);

    // Delete shaders now that they're linked in the shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    // Matrices: create, get their uniform locations
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -100.0f)); // Create vuiew matrix (for "camera")
    if ((viewMatrixLocation = glGetUniformLocation(shaderProgram, "view")) == -1) // Get location
        throw std::runtime_error("Failed to get view matrix location in shader program.");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix)); // Send it to shader program uniform (do it here, once, since will never change)

    projectionMatrix = glm::perspective(glm::radians(45.0f), static_cast<float>(Config::WindowWidth) / Config::WindowHeight, 0.1f, 1000.0f); // For clipping and depth
    if ((projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projection")) == -1)
        throw std::runtime_error("Failed to get projection matrix location in shader program.");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    if ((modelMatrixLocation = glGetUniformLocation(shaderProgram, "model")) == -1)
        throw std::runtime_error("Failed to get model matrix location in shader program.");


    // Get sprite texture uniform location
    if ((spriteTextureLocation = glGetUniformLocation(shaderProgram, "spriteTexture")) == -1)
        throw std::runtime_error("Failed to get sprite texture location in shader program.");

    glUniform1i(spriteTextureLocation, 0); // Set which sampler unit the uniform (sampler2D) belongs to (0 = GL_TEXTURE0)
}



void Scene::SetupSprites()
{
    Sprite::InitializeOpenGL();
    sprite1.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    sprite2.SetPosition(glm::vec3(20.0f, 20.0f, -1.0f));
    sprite3.SetPosition(glm::vec3(40.0f, 40.0f, -2.0f));
    sprite1.LoadTexture("default.png");
    sprite2.LoadTexture("testimage.png");
    sprite3.LoadTexture("testimage2.png");

}

#include <SDL3/SDL_time.h>
void Scene::DrawSprites()
{
    glUseProgram(shaderProgram);
    glBindVertexArray(Sprite::VAO);

    Uint64 ticks = SDL_GetTicks();
    sprite1.SetRotation(ticks / 5.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    sprite2.SetRotation(ticks / 5.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    sprite3.SetRotation(ticks / 5.0f, glm::vec3(0.5f, 0.5f, 0.0f));

//    int nrOfVertices = 6;

    glActiveTexture(GL_TEXTURE0); // Set which texture unit to bind textures to

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(sprite1.GetModelMatrix())); // Send model matrix to vertex shader (as uniform)
    glBindTexture(GL_TEXTURE_2D, sprite1.m_Texture);
    glDrawElements(GL_TRIANGLES, Sprite::nrOfVertexIndices, GL_UNSIGNED_INT, 0);

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(sprite2.GetModelMatrix()));
    glBindTexture(GL_TEXTURE_2D, sprite2.m_Texture);
    glDrawElements(GL_TRIANGLES, Sprite::nrOfVertexIndices, GL_UNSIGNED_INT, 0);

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(sprite3.GetModelMatrix()));
    glBindTexture(GL_TEXTURE_2D, sprite3.m_Texture);
    glDrawElements(GL_TRIANGLES, Sprite::nrOfVertexIndices, GL_UNSIGNED_INT, 0);
//    glDrawArrays(GL_TRIANGLES, 2 * nrOfVertices, nrOfVertices);

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


        DrawSprites();

        SDL_GL_SwapWindow(m_Window.get());

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


Scene::~Scene()
{
    glDeleteVertexArrays(1, &Sprite::VAO);
    glDeleteBuffers(1, &Sprite::VBO);
    glDeleteProgram(shaderProgram);
}
