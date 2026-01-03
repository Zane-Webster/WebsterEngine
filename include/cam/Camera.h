#ifndef WE_CAM_CAMERA_H_
#define WE_CAM_CAMERA_H_

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/WebsterEngine.h"
#include "core/Logger.h"
#include "utils/Utils.h"

class Camera {
public:
    Camera(float aspect_ratio, WE::KEYSET keyset, std::shared_ptr<double> delta_time);

    // ======== MATRICES ========
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetViewProjectionMatrix();

    // ======== POSITION ========
    void SetPosition(glm::vec3 new_position);
    glm::vec3 GetPosition();

    void Translate(glm::vec3 translation);

    // ======== KEYBOARD ========
    void StartKey(SDL_Scancode scancode);
    bool ProcessKey();
    void EndKey(SDL_Scancode scancode);
    void StopAllKey();

    // ======== MOUSE ========
    void LookAt(glm::vec3 target);

    bool ProcessMouse(float x, float y);

private:
    WE::KEYSET keyset = WE::KEYSET::WASD;
    std::shared_ptr<double> delta_time = nullptr;

    std::vector<SDL_Scancode> keys = {};

    std::unique_ptr<glm::vec3> position = std::make_unique<glm::vec3>(0.0f, 0.0f, 3.0f);

    const glm::vec3 true_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;

    float fov = 90.0f;
    float aspect_ratio = (16.0f / 9.0f);
    float near_plane = 0.1f;
    float far_plane = 1000.0f;

    float max_speed = 5.0f;
    float acceleration = 1.0f;

    float mouse_sens = 0.1f;

    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix = glm::mat4(1.0f);
    glm::mat4 view_projection_matrix = glm::mat4(1.0f);
    
    void _HandleMovement();

    void _UpdateViewMatrix();
    void _UpdateProjectionMatrix();
    void _UpdateMatrices();
    void _UpdateVectors();
};

#endif // WE_CAM_CAMERA_H_