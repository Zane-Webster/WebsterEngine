#include "cam/Camera.h"

Camera::Camera(float p_aspect_ratio, WE::KEYSET p_keyset) : aspect_ratio(p_aspect_ratio), keyset(p_keyset) {
    ;
}

//=============================
// MATRICES
//=============================
glm::mat4 Camera::GetViewMatrix() {
    return view_matrix;
}

glm::mat4 Camera::GetProjectionMatrix() {
    return projection_matrix;
}

glm::mat4 Camera::GetViewProjectionMatrix() {
    return projection_matrix * view_matrix;
}

void Camera::_UpdateViewMatrix() {
    ;
}

void Camera::_UpdateProjectionMatrix() {
    ;
}


//=============================
// POSITION
//=============================
void Camera::SetPosition(glm::vec3 new_position) {
    ;
}

glm::vec3 Camera::GetPosition() {
    return *position;
}

void Camera::Translate(glm::vec3 translation) {
    ;
}

//=============================
// KEYBOARD
//=============================
void Camera::StartKey(SDL_Scancode scancode) {
    Logger::Debug(Utils::MovementFromScancode(keyset, scancode));
}

bool Camera::ProcessKey() {
    return false;
}

void Camera::EndKey(SDL_Scancode scancode) {
    Logger::Debug(Utils::MovementFromScancode(keyset, scancode));
}

void Camera::StopAllKey() {
    ;
}

//=============================
// MOUSE
//=============================
void Camera::LookAt(glm::vec3 target) {
    ;
}

void Camera::SetYawPitch(float p_yaw, float p_pitch) {
    ;
}

bool Camera::ProcessMouse(float x, float y) {
    return false;
}