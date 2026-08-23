#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL3/SDL_render.h>
#include <memory>
#include "Transform.hpp"
#include <glad/gl.h>

//struct TextureDestroyer
//{
//    void operator()(SDL_Texture* texture){
//        SDL_DestroyTexture(texture);
//    }
//};

class Sprite
{
    public:
        Sprite();
        virtual ~Sprite();

//        void Draw(SDL_Renderer* renderer);

        GLuint m_Texture = 0; // 0 means no texture

        void LoadTexture(const std::string& fileName); // Loads image (png) to GPU

//        SDL_Texture* GetTexture() { return m_Texture.get(); }

//        void UpdateVBO();

        float position_x = 0;
        float position_y = 0;
        int width = 50;
        int height = 40;

        Transform GetTransform() {return m_Transform;}
        void SetTransform(Transform p_Transform) {m_Transform = p_Transform; UpdateModelMatrix();}
        void SetPosition(glm::vec3 position) {m_Transform.Position = position; UpdateModelMatrix();}
        void SetRotation(float angleInDegrees, glm::vec3 axis) {
            m_Transform.SetRotation(angleInDegrees, axis);
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
//        void UploadVerticesToVBO();
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

        static inline unsigned int nrOfSprites = 0;
        static const int MAX_NR_OF_SPRITES = 1000;
        unsigned int spriteNr = nrOfSprites; // 1st sprite: spriteNr=0, 2nd sprite: spriteNr=1, ...


    protected:

    private:
        Transform m_Transform; // Describes how to transform this Sprite's vertices from local space to world space
        glm::mat4 modelMatrix = m_Transform.ToModelMatrix();
//        std::unique_ptr<SDL_Texture, TextureDestroyer> m_Texture;
};

#endif // SPRITE_HPP
