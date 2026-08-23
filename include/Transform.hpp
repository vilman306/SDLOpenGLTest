#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>


struct Transform
{
glm::vec3 Position = {0.0f, 0.0f, 0.0f};
glm::quat Rotation = {1.0f, 0.0f, 0.0f, 0.0f};
glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

glm::mat4 ToModelMatrix()
{
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), Position);
    glm::mat4 rotation = glm::mat4_cast(Rotation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), Scale);
    return translation * rotation * scale;
}

void SetRotation(float angleInDegrees, glm::vec3 axis)
{
    Rotation = glm::angleAxis(glm::radians(angleInDegrees), glm::normalize(axis));
}

};
