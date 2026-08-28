#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL3/SDL_render.h>
#include <memory>
#include "Transform.hpp"
#include <glad/gl.h>
#include "Mesh.hpp"

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
//            InitializeStatics();
            LoadTexture(textureFileName);
            SetNormal(normal);
            nrOfSprites++;
        }
        Sprite(const std::string& textureFileName, glm::vec3 normal, glm::vec3 position)
        {
//            InitializeStatics();
            LoadTexture(textureFileName);
            SetPosition(position);
            SetNormal(normal);
            nrOfSprites++;
        }
        Sprite(const std::string& textureFileName, glm::vec3 normal, glm::vec3 position, glm::vec3 scale)
        {
//            InitializeStatics();
            LoadTexture(textureFileName);
            SetPosition(position);
            SetScale(scale);
            SetNormal(normal);
            nrOfSprites++;
        }

        // Prohibit copying (because GLTexture cannot be copied)
        Sprite(const Sprite& other) = delete;
        Sprite& operator=(const Sprite& other) = delete;

        Sprite(Sprite&& other) = default;
        Sprite& operator=(Sprite&& other) = default;

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

        void SetNormal(glm::vec3 targetNormal);

        static constexpr float sizeScaleFactor = 0.1f; // Used in UpdateModelMatrix to determine how the sprite's width and height affects scale.

        glm::mat4 GetModelMatrix() {return modelMatrix;}
        void UpdateModelMatrix();

//        static void InitializeOpenGL(); // Call once, this creates the VBO, VAO etc

//        static inline GLuint VBO; // shared vertex buffer object
//        static inline GLuint VAO; // shared vertex array object
//        static inline GLuint EBO; // shared element buffer object
//        static inline bool hasInitializedStatics = false; // True if has generated VBO and VAO, see constructor
//
//        static const unsigned int nrOfVertices = 4; // nr of vertices when using an element buffer object
//        static const unsigned int nrOfVertexIndices = 6; // -- || -- when not using an element buffer object
//
//                                                                        // position         // texture coord
//        static inline const std::array<float, nrOfVertices * (3+2)> vertices =
//                                                                       {-0.5f, 0.5f, 0.0f,      0.0f, 1.0f, // top left
//                                                                        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f, // bottom left
//                                                                        0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right
//                                                                        0.5f, 0.5f, 0.0f,       1.0f, 1.0f}; // top right
//        static inline const std::array<GLuint, nrOfVertexIndices> vertexIndices = {0, 1, 2, 2, 3, 0};

//        void InitializeStatics();
        static Mesh& GetMesh();

        static inline unsigned int nrOfSprites = 0;
        static const int MAX_NR_OF_SPRITES = 1000;
        static inline bool hasInitializedStatics = false;


    protected:
        Transform m_Transform; // Describes how to transform this Sprite's vertices from local space to world space
        glm::mat4 modelMatrix = m_Transform.ToModelMatrix();
        unsigned int m_SpriteNr = nrOfSprites; // 1st sprite: m_SpriteNr=0, 2nd sprite: m_SpriteNr=1, ...
        static inline Mesh m_Mesh;

        glm::vec3 m_Normal = {0.0f, 0.0f, 1.0f};
};

#endif // SPRITE_HPP
