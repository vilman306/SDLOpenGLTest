#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Config.h"
#include <stdexcept>
#include <iostream>
#include "InputSystem.hpp"

class Camera
{
    public:
        Camera();
        virtual ~Camera();

        glm::mat4 GetViewMatrix() { return m_ViewMatrix; }
        void SetViewMatrix(glm::mat4 val) { m_ViewMatrix = val; }
        glm::mat4 GetProjectionMatrix() { return m_ProjectionMatrix; }
        void SetProjectionMatrix(glm::mat4 val) { m_ProjectionMatrix = val; }
//
//        void Translate(glm::vec3 translationVector)
//        {
//            if (glm::length(translationVector) == 0.0f)
//                throw std::runtime_error("Trying to translate along an axis that is a zero vector.");
//            m_ViewMatrix = glm::translate(m_ViewMatrix, -translationVector);
//        }
//        void Translate(float amount, glm::vec3 axis)
//        {
//            if (glm::length(axis) == 0.0f)
//                throw std::runtime_error("Trying to translate along an axis that is a zero vector.");
//            m_ViewMatrix = glm::translate(m_ViewMatrix, -amount * glm::normalize(axis));
//        }
//        void Rotate(float angleInDegrees, glm::vec3 axis)
//        {
//            if (glm::length(axis) == 0.0f)
//                throw std::runtime_error("Trying to translate along an axis that is a zero vector.");
//            m_ViewMatrix = glm::rotate(m_ViewMatrix, glm::radians(angleInDegrees), glm::normalize(axis));
//        }
        void Update(CameraInputState input, double deltaTime)
        {
            MoveHorizontally(glm::vec2(input.MoveRight, input.MoveForward), static_cast<float>(deltaTime));
            MoveUp(input.MoveUp, deltaTime);
            Look(input.LookDeltaX, input.LookDeltaY);
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
        void SetPosition(glm::vec3 pos)
        {
            m_Position = pos;
            UpdateViewMatrix();
        }
        void UpdateViewMatrix()
        {
            m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        }
        glm::vec3 GetRightDir()
        {
            return glm::normalize(glm::cross(m_Front, m_Up));
        }
        void MoveHorizontally(glm::vec2 inputXZ, double deltaTime)
        {
            if (glm::length(inputXZ) == 0.0f)
                return;
            glm::vec3 movementAxis = glm::normalize(inputXZ.y * glm::vec3(m_Front.x, 0.0f, m_Front.z) + inputXZ.x * GetRightDir()); // "FPS" movement
//            glm::vec3 movementAxis = glm::normalize(inputXZ.y * m_Front + inputXZ.x * GetRightDir()); // "Fly" movement
            m_Position += movementAxis * movementSpeed * static_cast<float>(deltaTime);
            UpdateViewMatrix();
        }
        void MoveUp(float value, double deltaTime)
        {
            m_Position += value * m_Up * movementSpeed * static_cast<float>(deltaTime);
        }
        void Look(float mouse_dx, float mouse_dy)
        {
            yaw += mouse_dx * lookSensitivity;
            yaw = std::fmod(yaw, 360.0f); // Limit yaw to (-360.0, 360.0f)
            pitch += mouse_dy * lookSensitivity;
            if (pitch > maxPitch)
                pitch = maxPitch;
            else if (pitch < minPitch)
                pitch = minPitch;
            // Update Front direction, then update view matrix
            m_Front = FrontVectorFromYawPitch();
            UpdateViewMatrix();
        }
        glm::vec3 FrontVectorFromYawPitch()
        {
            return glm::normalize(glm::vec3(glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
                                                     glm::sin(glm::radians(pitch)),
                                                     glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch))));
        }
//        glm::vec3 FrontVectorFromYaw()
//        {
//            return glm::normalize(glm::vec3(glm::cos(glm::radians(yaw)),
//                                                     0.0f ,
//                                                     glm::sin(glm::radians(yaw))));
//        }


    protected:

    private:
        glm::vec3 m_Position = {0.0f, 0.0f, 100.0f};
//        glm::vec3 m_Target = {0.0f, -1.0f, 0.0f};
        float yaw = -90.0f; // 0 means +x direction, -90 means -z, see https://learnopengl.com/Getting-started/Camera
        float pitch = 0.0f; // 0 means in perpendicular to y axis, > 0 means above xz plane
        float minPitch = -89.0f;
        float maxPitch = 89.0f;
        glm::vec3 m_Front = FrontVectorFromYawPitch();
        glm::vec3 m_Up = {0.0f, 1.0f, 0.0f}; // Used in cross product for calculating right vector in glm::lookAt (see https://learnopengl.com/Getting-started/Camera)
        glm::mat4 m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up); // glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        float m_FOV = 90.0f; // degrees
        float m_NearPlaneDistance = 0.1f;
        float m_FarPlaneDistance = 1000.0f;
        float movementSpeed = 100.0f;
        float lookSensitivity = 0.15f;
        glm::mat4 m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), Config::AspectRatio, m_NearPlaneDistance, m_FarPlaneDistance);
};
#endif // CAMERA_HPP
