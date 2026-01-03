#include "cam/Camera.h"

Camera::Camera(float p_aspect_ratio, WE::KEYSET p_keyset, std::shared_ptr<double> p_delta_time) : aspect_ratio(p_aspect_ratio), keyset(p_keyset), delta_time(p_delta_time) {
    Camera::_UpdateVectors();
    Camera::_UpdateMatrices();
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
    return view_projection_matrix;
}

void Camera::_UpdateViewMatrix() {
    view_matrix = glm::lookAt(*position, *position + front, up);
    view_projection_matrix = projection_matrix * view_matrix;
}

void Camera::_UpdateProjectionMatrix() {
    projection_matrix = glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
    view_projection_matrix = projection_matrix * view_matrix;
}

void Camera::_UpdateMatrices() {
    view_matrix = glm::lookAt(*position, *position + front, up);
    projection_matrix = glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
    view_projection_matrix = projection_matrix * view_matrix;
}

//=============================
// POSITION
//=============================
glm::vec3 Camera::GetPosition() {
    return *position;
}

void Camera::Translate(glm::vec3 translation) {
    *position += translation;
    _UpdateViewMatrix();
}

//=============================
// KEYBOARD
//=============================
void Camera::StartKey(SDL_Scancode scancode) {
    if (!Utils::VectorContains(keys, scancode)) keys.push_back(scancode);
}

bool Camera::ProcessKey() {
    if (keys.empty()) return false;

    for (auto& key : keys) {
        //
    }

    return true;
}

void Camera::_HandleMovement() {
    ;
}

void Camera::EndKey(SDL_Scancode scancode) {
    if (Utils::VectorContains(keys, scancode)) std::erase(keys, scancode);
}

void Camera::StopAllKey() {
    keys = {};
}

//=============================
// MOUSE
//=============================
void Camera::LookAt(glm::vec3 target) {
    glm::vec3 direction = glm::normalize(target - *position);

    pitch = glm::degrees(asin(direction.y));
    yaw = glm::degrees(atan2(direction.z, direction.x)) - 90.0f;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    _UpdateVectors();
    _UpdateViewMatrix();
}

bool Camera::ProcessMouse(float x, float y) {
    if (x == 0.0f && y == 0.0f) return false;

    x *= mouse_sens;
    y *= mouse_sens;

    yaw += x;

    pitch = glm::clamp((pitch - y), -89.0f, 89.0f);

    Camera::_UpdateVectors();
    Camera::_UpdateViewMatrix();

    return true;
}

void Camera::_UpdateVectors() {
    glm::vec3 f = glm::vec3(0.0f);
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, true_up));
    up = glm::normalize(glm::cross(right, front));
}