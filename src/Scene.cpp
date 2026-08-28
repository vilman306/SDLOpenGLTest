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
//        if (!SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24))
//            throw std::runtime_error(SDL_GetError());
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

        if (!SDL_SetWindowRelativeMouseMode(window, true))
            throw std::runtime_error("Failure on SDL_SetWindowRelativeMouseMode: " + std::string(SDL_GetError()));

        // window and glContext OK, store/own them.
        m_Window.reset(window);
        m_GLContext.reset(glContext);

        int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress); // Initializes GLAD?
        if (version == 0)
            throw std::runtime_error("Failed to initialize GLAD");
        std::cout << "GL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
//
//        GLint depthBits = 0;
//        glGetFramebufferAttachmentParameteriv(
//            GL_FRAMEBUFFER,
//            GL_DEPTH,
//            GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,
//            &depthBits
//        );
//        std::cout << "Depth buffer bits: " << depthBits << std::endl;

        SetupOpenGL();
        SetupSprites();
    }

    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        m_IsValid = false;
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

#include "GLUtils.hpp"
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
        throw std::runtime_error(std::string("Failed to compile shader of type ") + GLUtils::GLTypeToString(shaderType) + " :" + infoLog);
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
//    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -100.0f)); // Create vuiew matrix (for "camera")
    if ((viewMatrixLocation = glGetUniformLocation(shaderProgram, "view")) == -1) // Get location
        throw std::runtime_error("Failed to get view matrix location in shader program.");
//    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix)); // Send it to shader program uniform (do it here, once, since will never change)

//    float fovDegrees = 90.0f;
//    projectionMatrix = glm::perspective(glm::radians(fovDegrees), static_cast<float>(Config::WindowWidth) / Config::WindowHeight, 0.1f, 1000.0f); // For clipping and depth
    if ((projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projection")) == -1)
        throw std::runtime_error("Failed to get projection matrix location in shader program.");
//    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    if ((modelMatrixLocation = glGetUniformLocation(shaderProgram, "model")) == -1)
        throw std::runtime_error("Failed to get model matrix location in shader program.");


    // Get sprite texture uniform location
    if ((spriteTextureLocation = glGetUniformLocation(shaderProgram, "spriteTexture")) == -1)
        throw std::runtime_error("Failed to get sprite texture location in shader program.");

    glUniform1i(spriteTextureLocation, 0); // Set which sampler unit the uniform (sampler2D) belongs to (0 = GL_TEXTURE0)


    glEnable(GL_DEPTH_TEST); // Enable depth testing


    glEnable(GL_BLEND); // Enable blending, so can display transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set source and destination factors, see https://learnopengl.com/Advanced-OpenGL/Blending
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
        m_Sprites.push_back(std::make_unique<Sprite>(std::move(boxSprite)));
    }
}

void Scene::SetupSprites()
{
    Sprite::InitializeOpenGL();

    CreateBox("alphatest.png", glm::vec3(0.0f, 0.0f, -50.0f));
    CreateBox("cubeside.png", glm::vec3(-50.0f, 0.0f, -50.0f));
    CreateBox("default.png", glm::vec3(50.0f, 0.0f, -50.0f));
    Sprite ground("ground.png", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -50.0f, 0.0f), glm::vec3(40.0f, 40.0f, 1.0f));
    m_Sprites.push_back(std::make_unique<Sprite>(std::move(ground)));
//    CreateBox("lightsource.png", glm::vec3(0.0f, 30.0f, 25.0f));


}

#include <SDL3/SDL_time.h>
#include <algorithm>
void Scene::DrawSprites(Camera& camera)
{
    // Begin drawing sprites
    glUseProgram(shaderProgram);
    glBindVertexArray(Sprite::VAO);

    glActiveTexture(GL_TEXTURE0); // Set which texture unit to bind textures to

    // Send camera matrices to shader program's uniforms
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix())); // View matrix
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix())); // Perspective matrix

    // Sort sprites based on distance (needed for drawing transparent objects correclty: https://learnopengl.com/Advanced-OpenGL/Blending)
    std::vector<Sprite*> spritesSortedByDistance;
    for (std::unique_ptr<Sprite>& sprite: m_Sprites)
        spritesSortedByDistance.push_back(sprite.get());

    std::sort( spritesSortedByDistance.begin(), spritesSortedByDistance.end(),
            [&camera](const Sprite* a, const Sprite* b) {
                float aToCameraDist = glm::length(camera.GetPosition() - a->GetPosition());
                float bToCameraDist = glm::length(camera.GetPosition() - b->GetPosition());
                return aToCameraDist > bToCameraDist; // Sort in descending order (want to draw objects furthest away first)
            } );

    for (Sprite* sprite : spritesSortedByDistance) {
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(sprite->GetModelMatrix())); // Send model matrix to vertex shader (as uniform)
        glBindTexture(GL_TEXTURE_2D, sprite->m_Texture.GetID());
        glDrawElements(GL_TRIANGLES, Sprite::nrOfVertexIndices, GL_UNSIGNED_INT, 0);
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
    glDeleteVertexArrays(1, &Sprite::VAO);
    glDeleteBuffers(1, &Sprite::VBO);
    glDeleteProgram(shaderProgram);
}
