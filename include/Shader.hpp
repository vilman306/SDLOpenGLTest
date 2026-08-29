#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glad/gl.h>

enum class ShaderType
{
    Sprite
};

class Shader
{
    public:
        Shader(const std::string& vertexName, const std::string& fragmentName);
        ~Shader() { glDeleteProgram(shaderProgram); }

        // Prohibit copying (to prevent mulitple deletions of shaderProgram)
        Shader(const Shader& other) = delete;
        Shader& operator=(const Shader& other) = delete;

        Shader(Shader&& other) noexcept {
            shaderProgram = other.shaderProgram;
            other.shaderProgram = 0;
        }
        Shader& operator=(Shader&& other) noexcept {
            if (this != &other) {
                glDeleteProgram(shaderProgram);
                shaderProgram = other.shaderProgram;
                other.shaderProgram = 0;
            }
            return *this;
        }


        void Use() {glUseProgram(shaderProgram);}
        template <typename T>
        void SetUniform(const std::string& name, const T& value) const;

    protected:
        GLint GetUniformLocation(const std::string& uniformName) const;
        GLuint shaderProgram = 0;

};

#endif // SHADER_HPP
