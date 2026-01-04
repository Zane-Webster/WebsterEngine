#ifndef WE_OBJ_OBJECT_H_
#define WE_OBJ_OBJECT_H_

#include <iostream>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "prim/Triangle.h"

#include "utils/Utils.h"

class Object {
public:
    Object(std::string name, std::vector<std::unique_ptr<Triangle>>&& triangles = WE_EMPTY_VECTOR);
    void Destroy();

    void Render();

    bool Raycast(WE::Ray ray, WE::RayHit& hit);

    glm::mat4 GetModelMatrix() { return *model_matrix; };
    WE::AABB GetAABB();

    std::string name = WE_EMPTY_STRING;

private:
    std::shared_ptr<glm::mat4> model_matrix = std::make_shared<glm::mat4>(1.0f);

    std::vector<std::unique_ptr<Triangle>> triangles = WE_EMPTY_VECTOR;

    WE::AABB aabb = {};

    void _CalculateAABB();

};

#endif // WE_OBJ_OBJECT_H_