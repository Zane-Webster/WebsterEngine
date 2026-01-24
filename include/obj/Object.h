#ifndef WE_OBJ_OBJECT_H_
#define WE_OBJ_OBJECT_H_

#include <iostream>
#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "prim/Triangle.h"

#include "utils/Utils.h"

class Object {
public:
    Object(std::string name, WE::Material material, std::vector<std::unique_ptr<Triangle>>&& triangles, WE::COLLIDER_TYPE collider_type = WE::COLLIDER_TYPE::UNDECLARED, glm::vec3 origin = glm::vec3(0.0f));

    // ======== BASIC ========
    void Render();
    void Destroy();

    // ======== POSITION ========
    void SetPosition(glm::vec3 position);
    virtual void Translate(glm::vec3 translation);

    void ResetToOrigin();

    glm::vec3 GetPosition() { return *position; };

    // ======== COLLISION ========
    WE::COLLIDER_TYPE GetColliderType() { return collider->type; };
    std::shared_ptr<WE::ColliderShape> GetCollider() { return collider; };

    // ======== RAYCASTING ========
    bool Raycast(WE::Ray ray, WE::RayHit& hit);

    // ======== COLOR ========
    void SetColor(glm::vec3 color);

    // ======== GETTERS ========
    glm::mat4 GetModelMatrix() { return *model_matrix; };
    WE::AABB GetAABB();
    glm::vec3 GetColor();

    std::string name = WE_EMPTY_STRING;
    WE::Material material;

    WE::AABB aabb = {};

protected:
    std::shared_ptr<glm::vec3> position = std::make_shared<glm::vec3>(0.0f);
    std::shared_ptr<glm::vec3> origin = std::make_shared<glm::vec3>(0.0f);

    std::unique_ptr<glm::vec3> size = std::make_unique<glm::vec3>(0.0f);
    std::unique_ptr<glm::vec3> center = std::make_unique<glm::vec3>(0.0f);

    std::shared_ptr<glm::mat4> model_matrix = std::make_shared<glm::mat4>(1.0f);
    std::shared_ptr<glm::mat4> origin_model_matrix = std::make_shared<glm::mat4>(1.0f);

    std::vector<std::unique_ptr<Triangle>> triangles = WE_EMPTY_VECTOR;

    glm::vec3 color = glm::vec3(0.25f);

    std::shared_ptr<WE::ColliderShape> collider = std::make_unique<WE::ColliderShape>(WE::COLLIDER_TYPE::AABB);

    void _CalculateAABB();
    void _UpdateModelMatrix();
    
    void _BuildCollider(WE::COLLIDER_TYPE type);
    void _UpdateCollider();
};

#endif // WE_OBJ_OBJECT_H_