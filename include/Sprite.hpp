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
        Sprite(GLuint p_VBO);
        virtual ~Sprite();

//        void Draw(SDL_Renderer* renderer);

        void LoadTexture(const std::string& filePath, SDL_Renderer* renderer); // Loads image (png) to GPU

        SDL_Texture* GetTexture() { return m_Texture.get(); }

        void UpdateVBO();

        float position_x = 20;
        float position_y = 30;
        int width = 0;
        int height = 0;

        Transform m_Transform; // Describes how to transform this Sprite's vertices from local space to world space
        Transform GetTransform() {return m_Transform;}
        void SetTransform(Transform p_Transform) {m_Transform = p_Transform; modelMatrix = m_Transform.ToModelMatrix();}

        glm::mat4 modelMatrix = m_Transform.ToModelMatrix();


        GLuint VBO;

        std::array<glm::vec3, 6> vertices;

        inline static unsigned int nrOfSprites = 0;

        unsigned int spriteNr = nrOfSprites; // 1st sprite: spriteNr=0, 2nd sprite: spriteNr=1, ...


    protected:

    private:
        std::unique_ptr<SDL_Texture, TextureDestroyer> m_Texture;
};

#endif // SPRITE_HPP
