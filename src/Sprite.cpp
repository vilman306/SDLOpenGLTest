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

GLenum GetColorFormat(int nrOfColorChannels)
{
    switch (nrOfColorChannels)
    {
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: throw std::runtime_error(std::string("Function nrOfColorChannelsToEnum has not defined a case for nrOfColorChannels == "
                                        + std::to_string(nrOfColorChannels) + "."));
    }
}

#include <stb_image.h>

void Sprite::LoadTexture(const std::string& filePath)
{
    std::string fullPath = std::string(SDL_GetBasePath()) + "/textures/" + filePath;
//    SDL_Surface* surface = SDL_LoadPNG(fullPath.c_str());
//    if (!surface) {
//        std::cout << "ERROR: failed to create surface from '" << fullPath << "'." << std::endl;
//        return;
//    }
//    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
//    if (!texture) {
//        std::cout << "ERROR: failde to create texture from surface (of '" << fullPath << "')." << std::endl;
//        return;
//    }
//    m_Texture.reset(texture);
//    width = surface->w;
//    height = surface->h;
//
//    SDL_DestroySurface(surface);

    // Load texture data from path
    int nrOfColorChannels;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrOfColorChannels, 0);
    if (!data)
        throw std::runtime_error("Failed to load texture data from '" + fullPath + "'.");

    // (create and) bind texture
    if (m_Texture == 0)
        glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    // set texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {0.7f, 0.0f, 0.3f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum colorFormat = GetColorFormat(nrOfColorChannels);
    glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

}
