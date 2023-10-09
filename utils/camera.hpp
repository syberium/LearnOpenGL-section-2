#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CameraDefaults
{
    const float YAW = -90.0;
    const float PITCH = 0.0;
    const float SPEED = 2.5;
    const float SENSITIVITY = 0.1;
    const float ZOOM = 45.0;
}

class Camera
{

public:
    enum Movement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
    };

    enum Mode
    {
        FLY,
        FPS,
    };

    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler angles
    float Yaw;
    float Pitch;
    // camera Options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Mode mode{FLY};

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0, 0.0, 0.0),
            glm::vec3 up = glm::vec3(0.0, 1.0, 0.0),
            float yaw = CameraDefaults::YAW, 
            float pitch = CameraDefaults::PITCH
        ) 
        : Position(std::move(position))
        , Front(glm::vec3(0.0, 0.0, -1.0))
        , WorldUp(std::move(up))
        , Yaw(yaw)
        , Pitch(pitch)
        , MovementSpeed(CameraDefaults::SPEED)
        , MouseSensitivity(CameraDefaults::SENSITIVITY)
        , Zoom(CameraDefaults::ZOOM)
    {
        updateVectors();
    }

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch
        )
        : Position(glm::vec3(posX, posY, posZ))
        , Front(glm::vec3(0.0, 0.0, -1.0))
        , WorldUp(glm::vec3(upX, upY, upZ))
        , Yaw(yaw)
        , Pitch(pitch)
        , MovementSpeed(CameraDefaults::SPEED)
        , MouseSensitivity(CameraDefaults::SENSITIVITY)
        , Zoom(CameraDefaults::ZOOM)
    {}
        
    // return the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
    // processes input received from any keyboard-like input system. 
    // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing system)
    void ProcessKeyboard(Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        glm::mat3 modeModifier = glm::mat3(1.0f);
        if (mode == Mode::FPS)
            modeModifier[1][1] = 0.0f;
        switch (direction)
        {
            case (Movement::FORWARD): 
                Position += modeModifier * Front * velocity;
                break;
            case (Movement::BACKWARD):
                Position -= modeModifier * Front * velocity;
                break;
            case (Movement::RIGHT):
                Position += modeModifier * Right * velocity;
                break;
            case (Movement::LEFT):
                Position -= modeModifier * Right * velocity;
                break;
        }
    }

    void ProcessMouseMovement(double xoffset, double yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;
        
        // make sure when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0) Pitch = 89.0;
            if (Pitch < -89.0) Pitch = -89.0;
        }

        // update Front, Right and Up vectors using the updated Euler angles
        updateVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= yoffset;
        if (Zoom < 1.0) Zoom = 1.0;
        if (Zoom > 45.0) Zoom = 45.0;
    }
private:
    void updateVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vectors
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};