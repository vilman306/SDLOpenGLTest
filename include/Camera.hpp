#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Config.h"
#include <stdexcept>
#include <iostream>

class Camera
{
    public:
        Camera();
        virtual ~Camera();

        glm::mat4 GetViewMatrix() { return m_ViewMatrix; }
        void SetViewMatrix(glm::mat4 val) { m_ViewMatrix = val; }
        glm::mat4 GetProjectionMatrix() { return m_ProjectionMatrix; }
        void SetProjectionMatrix(glm::mat4 val) { m_ProjectionMatrix = val; }

        void Translate(glm::vec3 translationVector)
        {
            if (glm::length(translationVector) == 0.0f)
                throw std::runtime_error("Trying to translate along an axis that is a zero vector.");
            m_ViewMatrix = glm::translate(m_ViewMatrix, -translationVector);
        }
        void Translate(float amount, glm::vec3 axis)
        {
            if (glm::length(axis) == 0.0f)
                throw std::runtime_error("Trying to translate along an axis that is a zero vector.");
            m_ViewMatrix = glm::translate(m_ViewMatrix, -amount * glm::normalize(axis));
        }
        void UpdateProjectionMatrix()
        {
            m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), Config::AspectRatio, m_NearPlaneDistance, m_FarPlaneDistance);
        }
        void SetFOV(float newFOVDegrees)
        {
            if (newFOVDegrees <= 0)
                throw std::runtime_error("Trying to set camera's FOV to a value less than or equal to 0 degrees.");
            m_FOV = newFOVDegrees;
            UpdateProjectionMatrix();
        }
        void SetNearPlaneDistance(float val)
        {
            if (val >= m_FarPlaneDistance)
                throw std::runtime_error("Trying to set camera's near plane distance to a value greater than or equal to its far plane distance.");
            m_NearPlaneDistance = val;
            UpdateProjectionMatrix();
        }
        void SetFarPlaneDistance(float val)
        {
            if (val <= m_NearPlaneDistance)
                throw std::runtime_error("Trying to set camera's far plane distance to a value less than or equal to its near plane distance.");
            m_FarPlaneDistance = val;
            UpdateProjectionMatrix();
        }
        void PrintViewMatrix() {
            std::cout << "\nStart of view matrix print!\n";
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    std::cout  << "Row " << i << " column " << j <<  ": " << m_ViewMatrix[i][j] << "\n";
                }
            }
            std::cout << "End of view matrix print!\n";
        }

    protected:

    private:
        glm::mat4 m_ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -100.0f));;
        float m_FOV = 45.0f; // degrees
        float m_NearPlaneDistance = 0.1f;
        float m_FarPlaneDistance = 1000.0f;
        glm::mat4 m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), Config::AspectRatio, m_NearPlaneDistance, m_FarPlaneDistance);
};
#endif // CAMERA_HPP
