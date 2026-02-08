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
void Camera::SetPosition(glm::vec3 p_position) {
    *position = p_position;
    _UpdateViewMatrix();
}

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
    glm::vec3 local = glm::vec3(0.0f);

    for (SDL_Scancode key : keys) {
        local += Utils::MovementFromScancode(keyset, key);
    }

    Camera::_HandleMovement(local);

    if (velocity != glm::vec3(0.0f)) {
        Translate(velocity * static_cast<float>(*delta_time));
        return true;
    }

    return false;
}

void Camera::_HandleMovement(glm::vec3 local) {
    float dt = static_cast<float>(*delta_time);

    if (local != glm::vec3(0.0f)) {
        // normalize diagonals
        local = glm::normalize(local);

        // switch to world (front facing rather than world axis aligned)
        glm::vec3 desired = front * local.z + right * local.x + up * local.y;

        // normalize diagonals
        desired = glm::normalize(desired);

        float alignment = glm::dot(velocity, desired);

        // =========================
        // SAME-AXIS DIRECTION SWITCH
        // e.g. switching from left to right, deceleration should be applied while slowing down, then accecleration should be applied in the next direction
        // =========================
        if (alignment < 0.0f) {
            // brake first
            float speed = glm::length(velocity);
            speed = std::max(speed - deceleration * dt, 0.0f);

            velocity = (speed > 0.0f)
                ? glm::normalize(velocity) * speed
                : glm::vec3(0.0f);
        }
        else {
            // =========================
            // NORMAL STEERING
            // =========================

            // decompose velocity
            glm::vec3 parallel = desired * alignment;
            glm::vec3 perpendicular = velocity - parallel;

            // accelerate forward
            parallel += desired * acceleration * dt;

            // decelerate sideways drift
            float perp_speed = glm::length(perpendicular);
            if (perp_speed > 0.0f) {
                float drop = deceleration * dt;
                perp_speed = std::max(perp_speed - drop, 0.0f);
                perpendicular = (perp_speed > 0.0f)
                    ? glm::normalize(perpendicular) * perp_speed
                    : glm::vec3(0.0f);
            }

            velocity = parallel + perpendicular;
        }

        // clamp max speed
        float speed = glm::length(velocity);
        if (speed > max_speed)
            velocity = glm::normalize(velocity) * max_speed;

    } else {
        // =========================
        // NO INPUT → FULL DECEL
        // =========================
        float speed = glm::length(velocity);
        if (speed > 0.0f) {
            speed = std::max(speed - deceleration * dt, 0.0f);
            velocity = (speed > 0.0f)
                ? glm::normalize(velocity) * speed
                : glm::vec3(0.0f);
        }
    }
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

//=============================
// RAYCASTING
//=============================
WE::Ray Camera::GetForwardRay() {
    return {*position, glm::normalize(front)};
}