#include "Sprite.hpp"
#include <SDL3/SDL_filesystem.h>
#include <string>
#include <iostream>

Sprite::Sprite()
{
    nrOfSprites++;

    // Change transform scale to match width and height

    // local axes
//    glm::vec3 rightDir = {1.0f, 0.0f, 0.0f};
//    glm::vec3 upDir = {0.0f, 1.0f, 0.0f};
//
//    // vertex positions in local coordinates
//    glm::vec3 topLeft = 0.5f * height * upDir - 0.5f * width * rightDir;
//    glm::vec3 topRight = 0.5f * height * upDir + 0.5f * width* rightDir;
//    glm::vec3 bottomLeft = -0.5f * height * upDir - 0.5f * width * rightDir;
//    glm::vec3 bottomRight = -0.5f * height * upDir + 0.5f * width * rightDir;
//
//    // vertices array in local coordinates
//    vertices = {topLeft, bottomLeft, bottomRight, topRight};
//    vertexIndices = {0, 1, 2, 2, 3, 4};

//    UpdateVBO();
}

Sprite::~Sprite()
{
    //dtor
}

void Sprite::InitializeOpenGL()
{
    if (hasInitializedStatics)
        throw std::runtime_error("Sprite::InitializeOpenGL has already been called.");

    glGenBuffers(1, &VBO); // Create vertex buffer object (for all sprites)
    glGenVertexArrays(1, &VAO); // Create vertex buffer object (for all sprites)
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

//    glBufferData(GL_ARRAY_BUFFER, MAX_NR_OF_SPRITES * nrOfVertices * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW); // Initialize buffer data with null
    glBufferData(GL_ARRAY_BUFFER, nrOfVertices * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nrOfVertexIndices * sizeof(GLuint), vertexIndices.data(), GL_STATIC_DRAW);

    // Specify vertex attributes
    GLuint positionAttribIndex = 0;
    glVertexAttribPointer(positionAttribIndex, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexArrayAttrib(VAO, positionAttribIndex);

    // Unbind buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    hasInitializedStatics = true;
}
//
//void Sprite::UploadVerticesToVBO()
//{
//    if (!hasInitializedStatics)
//        throw std::runtime_error("Trying to upload vertices to VBO before VBO has been created (Sprite::InitializeOpenGL() has not been called.");
//
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//                                                                    // does this work??
////    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
//         // does this work??
//    glBufferSubData(GL_ARRAY_BUFFER, spriteNr * vertices.size() * sizeof(glm::vec3), vertices.size() * sizeof(glm::vec3), vertices.data());
//}

//void Sprite::UpdateVBO()
//{
    // update the model matrix (which transforms from local space to world space)
    // instead of updating each vertex in the vbo


//    // local axes
//    glm::vec3 rightDir = {1.0f, 0.0f, 0.0f};
//    glm::vec3 upDir = {0.0f, 1.0f, 0.0f};
//
//    // vertex positions in local coordinates
//    glm::vec3 topLeft = 0.5f * height * upDir - 0.5f * width * rightDir;
//    glm::vec3 topRight = 0.5f * height * upDir + 0.5f * width* rightDir;
//    glm::vec3 bottomLeft = -0.5f * height * upDir - 0.5f * width * rightDir;
//    glm::vec3 bottomRight = -0.5f * height * upDir + 0.5f * width * rightDir;
//
//    // vertices array in local coordinates
//    vertices = {topLeft, bottomLeft, bottomRight,
//                bottomLeft, bottomRight, topRight};
//
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//         // does this work??
//    glBufferSubData(GL_ARRAY_BUFFER, spriteNr * vertices.size() * sizeof(glm::vec3), vertices.size() * sizeof(glm::vec3), vertices.data());
//}

//void Sprite::Draw(SDL_Renderer* renderer)
//{
//    if (!m_Texture) {
//        std::cout << "Trying to render sprite with no texture." << std::endl;
//    }
//    SDL_FRect destinationRect;
//    destinationRect.x = position_x;
//    destinationRect.y = position_y;
//    destinationRect.w = width;
//    destinationRect.h = height;
//    SDL_RenderTexture(renderer, m_Texture.get(), nullptr, &destinationRect);
//}

void Sprite::LoadTexture(const std::string& filePath, SDL_Renderer* renderer)
{
    std::string fullPath = std::string(SDL_GetBasePath()) + "/textures/" + filePath;
    SDL_Surface* surface = SDL_LoadPNG(fullPath.c_str());
    if (!surface) {
        std::cout << "ERROR: failed to create surface from '" << fullPath << "'." << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cout << "ERROR: failde to create texture from surface (of '" << fullPath << "')." << std::endl;
        return;
    }
    m_Texture.reset(texture);
    width = surface->w;
    height = surface->h;

    SDL_DestroySurface(surface);
}
