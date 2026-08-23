#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL3/SDL_render.h>
#include <memory>
#include "Transform.hpp"
#include <glad/gl.h>

struct TextureDestroyer
{
    void operator()(SDL_Texture* texture){
        SDL_DestroyTexture(texture);
    }
};

class Sprite
{
    public:
        Sprite();
        virtual ~Sprite();

//        void Draw(SDL_Renderer* renderer);

        void LoadTexture(const std::string& filePath, SDL_Renderer* renderer); // Loads image (png) to GPU

        SDL_Texture* GetTexture() { return m_Texture.get(); }

//        void UpdateVBO();

        float position_x = 20;
        float position_y = 30;
        int width = 100;
        int height = 100;

        Transform GetTransform() {return m_Transform;}
        void SetTransform(Transform p_Transform) {m_Transform = p_Transform; UpdateModelMatrix();}
        void SetPosition(glm::vec3 position) {m_Transform.Position = position; UpdateModelMatrix();}

        glm::mat4 modelMatrix = m_Transform.ToModelMatrix();
        void UpdateModelMatrix() {modelMatrix = m_Transform.ToModelMatrix();}

        static void InitializeOpenGL(); // Creates the
        void UploadVerticesToVBO();
        inline static GLuint VBO; // shared vertex buffer object
        inline static GLuint VAO; // shared vertex array object
        inline static bool hasInitializedStatics = false; // True if has generated VBO and VAO, see constructor

        std::array<glm::vec3, 6> vertices;

        inline static unsigned int nrOfSprites = 0;

        unsigned int spriteNr = nrOfSprites; // 1st sprite: spriteNr=0, 2nd sprite: spriteNr=1, ...


    protected:

    private:
        Transform m_Transform; // Describes how to transform this Sprite's vertices from local space to world space
        std::unique_ptr<SDL_Texture, TextureDestroyer> m_Texture;
};

#endif // SPRITE_HPP
