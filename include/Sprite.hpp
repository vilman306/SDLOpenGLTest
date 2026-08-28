#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL3/SDL_render.h>
#include <memory>
#include "Transform.hpp"
#include <glad/gl.h>

class GLTexture // exists so a sprite doesnt have a raw opengl texture handle (would require a long move constructor that copies a bunch of stuff)
{ // This class does not handle texture creation, only deletion.
    public:
        GLTexture() : m_ID(0) {};
        explicit GLTexture(GLuint tex) : m_ID(tex) {}; // https://www.geeksforgeeks.org/cpp/use-of-explicit-keyword-in-cpp/

        // forbid copying to avoid multiple deletions of a texture (deletion happens in destructor)
        GLTexture(const GLTexture& other) = delete;
        GLTexture& operator=(const GLTexture& other) = delete;

        GLTexture(GLTexture&& other) noexcept : m_ID(other.m_ID) {
            other.m_ID = 0;
        }
        GLTexture& operator=(GLTexture&& other) noexcept {
            if (this != &other) {
                Reset(); // Delete texture in case this hade one
                m_ID = other.m_ID;
                other.m_ID = 0;
            }
            return *this;
        }

        ~GLTexture()
        {
            Reset();
        }

        GLuint GetID() const { return m_ID; }

    private:
        void Reset() {
            if (m_ID != 0) {
                glDeleteTextures(1, &m_ID);
                m_ID = 0;
            }
        }
        GLuint m_ID = 0; // 0 means no texture
};

class Sprite
{
    public:
        Sprite();

        Sprite(const std::string& textureFileName, glm::vec3 normal)
        {
            LoadTexture(textureFileName);
            SetNormal(normal);
        }
        Sprite(const std::string& textureFileName, glm::vec3 normal, glm::vec3 position)
        {
            LoadTexture(textureFileName);
            SetPosition(position);
            SetNormal(normal);
        }
        Sprite(const std::string& textureFileName, glm::vec3 normal, glm::vec3 position, glm::vec3 scale)
        {
            LoadTexture(textureFileName);
            SetPosition(position);
            SetScale(scale);
            SetNormal(normal);
        }

        // Prohibit copying (because GLTexture cannot be copied)
        Sprite(const Sprite& other) = delete;
        Sprite& operator=(const Sprite& other) = delete;

        Sprite(Sprite&& other) = default;
        Sprite& operator=(Sprite&& other) = default;

        virtual ~Sprite() {
//            if (m_Texture != 0)
//                glDeleteTextures(1, &m_Texture); // GLTexture handles deletion
        }

       // GLuint m_Texture = 0; // 0 means no texture
        GLTexture m_Texture;

        void LoadTexture(const std::string& fileName); // Loads image (png) to GPU

        float position_x = 0;
        float position_y = 0;
        int width = 50;
        int height = 40;

        Transform GetTransform() {return m_Transform;}
        void SetTransform(Transform p_Transform) {m_Transform = p_Transform; UpdateModelMatrix();}
        void SetScale(glm::vec3 scale) {m_Transform.Scale = scale; UpdateModelMatrix();}
        void SetPosition(glm::vec3 position) {m_Transform.Position = position; UpdateModelMatrix();}
        void SetPosition(float x, float y, float z) {m_Transform.Position = glm::vec3(x,y,z); UpdateModelMatrix();}
        void SetRotation(float angleInDegrees, glm::vec3 axis) {
            m_Transform.SetRotation(angleInDegrees, axis);
            UpdateModelMatrix();
            // Recompute normal
            m_Normal = m_Transform.Rotation * glm::vec3(0.0f, 0.0f, 1.0f);
        }
        glm::vec3 GetPosition() const {return m_Transform.Position;}

        void SetNormal(glm::vec3 targetNormal)
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

        static constexpr float sizeScaleFactor = 0.1f; // Used in UpdateModelMatrix to determine how the sprite's width and height affects scale.

        glm::mat4 GetModelMatrix() {return modelMatrix;}
        void UpdateModelMatrix() {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Transform.Position);
            glm::mat4 rotation = glm::mat4_cast(m_Transform.Rotation);
            glm::mat4 transformScale = glm::scale(glm::mat4(1.0f), m_Transform.Scale);
            glm::mat4 sizeScale = glm::scale(glm::mat4(1.0f), sizeScaleFactor * glm::vec3(width, height, 0.0f));
            modelMatrix = translation * rotation * sizeScale * transformScale;
        }

        static void InitializeOpenGL(); // Call once, this creates the VBO, VAO etc

        static inline GLuint VBO; // shared vertex buffer object
        static inline GLuint VAO; // shared vertex array object
        static inline GLuint EBO; // shared element buffer object
        static inline bool hasInitializedStatics = false; // True if has generated VBO and VAO, see constructor

        static const unsigned int nrOfVertices = 4; // nr of vertices when using an element buffer object
        static const unsigned int nrOfVertexIndices = 6; // -- || -- when not using an element buffer object

                                                                        // position         // texture coord
        static inline const std::array<float, nrOfVertices * (3+2)> vertices =
                                                                       {-0.5f, 0.5f, 0.0f,      0.0f, 1.0f, // top left
                                                                        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f, // bottom left
                                                                        0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right
                                                                        0.5f, 0.5f, 0.0f,       1.0f, 1.0f}; // top right
        static inline const std::array<GLuint, nrOfVertexIndices> vertexIndices = {0, 1, 2, 2, 3, 0};
        glm::vec3 m_Normal = {0.0f, 0.0f, 1.0f};

        static inline unsigned int nrOfSprites = 0;
        static const int MAX_NR_OF_SPRITES = 1000;
        unsigned int spriteNr = nrOfSprites; // 1st sprite: spriteNr=0, 2nd sprite: spriteNr=1, ...


    protected:

    private:
        Transform m_Transform; // Describes how to transform this Sprite's vertices from local space to world space
        glm::mat4 modelMatrix = m_Transform.ToModelMatrix();
};

#endif // SPRITE_HPP
