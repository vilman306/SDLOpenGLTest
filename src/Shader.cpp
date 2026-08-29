#include "Shader.hpp"

#include <fstream>
#include <sstream>
#include "Config.h"

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
        throw std::runtime_error(std::string("Failed to compile shader of type ") + GLUtils::GLTypeToString(shaderType) + ": " + infoLog);
    }
    return shader;
}

Shader::Shader(const std::string& vertexName, const std::string& fragmentName)
{

    // Create shaders
    GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, Config::ShaderDirectory + vertexName);
    GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, Config::ShaderDirectory + fragmentName);

    // Create program, attach shaders, and link it
    shaderProgram = glCreateProgram();
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

    // Delete shaders now that they're linked in the shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GLint Shader::GetUniformLocation(const std::string& uniformName) const
{
    GLint uniformLocation;
    if ((uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str())) == -1) // Get location
        throw std::runtime_error("Failed to get uniform location of " + uniformName);
    return uniformLocation;
}

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

template<> void Shader::SetUniform<bool>(const std::string& name, const bool& value) const {
                glUniform1i(GetUniformLocation(name), static_cast<GLint>(value)); }

template<> void Shader::SetUniform<int>(const std::string& name, const int& value) const {
                glUniform1i(GetUniformLocation(name), static_cast<GLint>(value)); }

template<> void Shader::SetUniform<float>(const std::string& name, const float& value) const {
                glUniform1f(GetUniformLocation(name), static_cast<GLfloat>(value)); }

template<> void Shader::SetUniform<glm::vec2>(const std::string& name, const glm::vec2& value) const {
                glUniform2fv(GetUniformLocation(name), 1, &value[0]); }

template<> void Shader::SetUniform<glm::vec3>(const std::string& name, const glm::vec3& value) const {
                glUniform3fv(GetUniformLocation(name), 1, &value[0]); }

template<> void Shader::SetUniform<glm::vec4>(const std::string& name, const glm::vec4& value) const {
                glUniform4fv(GetUniformLocation(name), 1, &value[0]); }

template<> void Shader::SetUniform<glm::mat2>(const std::string& name, const glm::mat2& value) const {
                glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]); }

template<> void Shader::SetUniform<glm::mat3>(const std::string& name, const glm::mat3& value) const {
                glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]); }

template<> void Shader::SetUniform<glm::mat4>(const std::string& name, const glm::mat4& value) const {
                glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]); }
