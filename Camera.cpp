#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection,glm::vec3(0.0f, 1.0f, 0.)));
        cameraUpDirection = cameraUp;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        if (direction == MOVE_FORWARD)
        {
            cameraPosition += cameraFrontDirection * speed;
        }
        else if (direction == MOVE_BACKWARD)
        {
            cameraPosition -= cameraFrontDirection * speed;
        }
        else if (direction == MOVE_RIGHT)
        {
            cameraPosition += cameraRightDirection * speed;
        }
        else if (direction == MOVE_LEFT)
        {
            cameraPosition -= cameraRightDirection * speed;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float deltaPitch, float deltaYaw) {
        //TODO
        yaw += deltaYaw;

        // Limit pitch to avoid flipping
        if (pitch + deltaPitch < 89.0f && pitch + deltaPitch > -89.0f) {
            pitch += deltaPitch;
        }

        // Update the front, right, and up vectors based on the new pitch and yaw
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->cameraFrontDirection = glm::normalize(direction);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}