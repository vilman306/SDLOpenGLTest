#include "Sprite.hpp"
#include <SDL3/SDL_filesystem.h>
#include <string>
#include <iostream>

Sprite::Sprite()
{
//    InitializeStatics();
    if (nrOfSprites > MAX_NR_OF_SPRITES)
        throw std::runtime_error(std::string("There are more sprites than max limit: ") + std::to_string(MAX_NR_OF_SPRITES) + ".");

    nrOfSprites++;

}

Mesh& Sprite::GetMesh()
{
    static bool initialized = false;
    if (!initialized || m_Mesh.HasReset()) {
        m_Mesh = {
                std::vector<float> {-0.5f, 0.5f, 0.0f,      0.0f, 1.0f, // top left
                                   -0.5f, -0.5f, 0.0f,      0.0f, 0.0f, // bottom left
                                    0.5f, -0.5f, 0.0f,      1.0f, 0.0f, // bottom right
                                    0.5f, 0.5f, 0.0f,       1.0f, 1.0f}, // top right
                std::vector<GLuint> {0, 1, 2, 2, 3, 0},
                std::vector<VertexAttribute> { {3, GL_FLOAT, GL_FALSE}, {2, GL_FLOAT, GL_FALSE} }
        };
        initialized = true;
    }
    return m_Mesh;
}

//void Sprite::InitializeStatics()
//{
//    if (!hasInitializedStatics)
//    {
//        m_Mesh = {
//                std::vector<float> {-0.5f, 0.5f, 0.0f,      0.0f, 1.0f, // top left
//                                   -0.5f, -0.5f, 0.0f,    0.0f, 0.0f, // bottom left
//                                    0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right
//                                    0.5f, 0.5f, 0.0f,       1.0f, 1.0f}, // top right
//                std::vector<GLuint> {0, 1, 2, 2, 3, 0},
//                std::vector<VertexAttribute> { {3, GL_FLOAT, GL_FALSE}, {2, GL_FLOAT, GL_FALSE} }
//        };
//    }
//}

void Sprite::SetNormal(glm::vec3 targetNormal)
{
    if (glm::length(targetNormal) == 0.0f)
        throw std::runtime_error("Sprite: trying to set normal direction to a zero vector");
    targetNormal = glm::normalize(targetNormal);

    glm::vec3 rotAxis = glm::cross(m_Normal, targetNormal);
    float dotProduct = glm::dot(targetNormal, m_Normal);

    if (glm::length(rotAxis) < 1e-6f) { // Target and current normal are ~ aligned
        if (dotProduct > 0.0f) // Target and current normal are ~ parallell, don't do anything
            return;

        // Target and current normal are ~ anti parallell

        // WARNING: This code segment is AI generated and has not been tested
        // Rotate around arbitrary axis 180 degrees, update normal and model matrix
        glm::vec3 arbitraryAxis = (std::abs(m_Normal.x) < 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 perpendicularAxis = glm::normalize(glm::cross(m_Normal, arbitraryAxis));
        glm::quat deltaRotation = glm::angleAxis(glm::radians(180.0f), perpendicularAxis);
        m_Transform.Rotation = deltaRotation * m_Transform.Rotation;
        m_Normal = targetNormal;
        UpdateModelMatrix();
        return;
    }
    float rotAngle = glm::acos(glm::clamp(dotProduct, -1.0f, 1.0f)); // clamp in case floating point error causes dotProduct to leave range [-1.0f, 1.0f]. acos(dotProduct) works because targetNormal and m_Normal are normalized. dotProduct = cos(rotAngle) * len1 * len2
    // Rotate, update normal and model matrix
    glm::quat deltaRotation = glm::angleAxis(rotAngle, glm::normalize(rotAxis));
    m_Transform.Rotation = deltaRotation * m_Transform.Rotation;
    m_Normal = targetNormal;
    UpdateModelMatrix();

}

void Sprite::UpdateModelMatrix()
{
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Transform.Position);
    glm::mat4 rotation = glm::mat4_cast(m_Transform.Rotation);
    glm::mat4 transformScale = glm::scale(glm::mat4(1.0f), m_Transform.Scale);
    glm::mat4 sizeScale = glm::scale(glm::mat4(1.0f), sizeScaleFactor * glm::vec3(width, height, 0.0f));
    modelMatrix = translation * rotation * sizeScale * transformScale;
}

//void Sprite::InitializeOpenGL()
//{
//    if (hasInitializedStatics)
//        throw std::runtime_error("Sprite::InitializeOpenGL has already been called.");
//
//    glGenBuffers(1, &VBO); // Create vertex buffer object (for all sprites)
//    glGenVertexArrays(1, &VAO); // Create vertex buffer object (for all sprites)
//    glGenBuffers(1, &EBO);
//
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//
////    glBufferData(GL_ARRAY_BUFFER, MAX_NR_OF_SPRITES * nrOfVertices * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW); // Initialize buffer data with null
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices.data(), GL_STATIC_DRAW);
//
//    // Specify position vertex attributes
//    GLuint positionAttribIndex = 0;
//    glVertexAttribPointer(positionAttribIndex, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
//    glEnableVertexArrayAttrib(VAO, positionAttribIndex);
//
//    // Specify texture coordinate vertex attributes
//    GLuint textureCoordAttribIndex = 1;
//    glVertexAttribPointer(textureCoordAttribIndex, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
//    glEnableVertexArrayAttrib(VAO, textureCoordAttribIndex);
//
//    // Unbind buffers
//    glBindVertexArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//    hasInitializedStatics = true;
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

void Sprite::LoadTexture(const std::string& fileName)
{
    std::string fullPath = std::string(SDL_GetBasePath()) + "/textures/" + fileName;

    // Load & store image data from path
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture
    int nrOfColorChannels;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrOfColorChannels, 0);
    if (!data)
        throw std::runtime_error("Failed to load texture data from '" + fullPath + "'.");

    // (create and) bind texture
    GLuint textureID = 0;
    if (m_Texture.GetID() == 0) {
        glGenTextures(1, &textureID);
        m_Texture = GLTexture(textureID);
    }
    glBindTexture(GL_TEXTURE_2D, m_Texture.GetID());

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

    stbi_image_free(data); // Free the loaded image data
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture

    UpdateModelMatrix(); // Update model matrix, since width and height has changed (affects scale)

}
