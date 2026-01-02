#include "cam/Camera.h"

Camera::Camera(float p_aspect_ratio) : aspect_ratio(p_aspect_ratio) {
    ;
}

glm::mat4 Camera::GetViewMatrix() {
    ;
}

glm::mat4 Camera::GetProjectionMatrix() {
    ;
}

glm::mat4 Camera::GetViewProjectionMatrix() {
    ;
}

void Camera::SetPosition(glm::vec3 new_position) {
    ;
}

glm::vec3 Camera::GetPosition() {
    ;
}

void Camera::Translate(glm::vec3 translation) {
    ;
}

void Camera::ProcessKeyboard() {
    ;
}

void Camera::LookAt(glm::vec3 target) {
    ;
}

void Camera::SetYawPitch(float yaw, float pitch) {
    ;
}

void Camera::ProcessMouse(float x, float y) {
    ;
}

void Camera::_UpdateViewMatrix() {
    ;
}

void Camera::_UpdateProjectionMatrix() {
    ;
}
