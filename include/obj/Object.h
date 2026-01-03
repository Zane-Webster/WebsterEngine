#ifndef WE_OBJ_OBJECT_H_
#define WE_OBJ_OBJECT_H_

#include <iostream>
#include <memory>
#include <vector>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "prim/Triangle.h"

class Object {
public:
    Object(std::string name, std::vector<std::unique_ptr<Triangle>>&& triangles = WE_EMPTY_VECTOR);
    void Destroy();

    void Render();

    glm::mat4 GetModelMatrix() { return *model_matrix; };

    std::string name = WE_EMPTY_STRING;

private:
    std::shared_ptr<glm::mat4> model_matrix = std::make_shared<glm::mat4>(1.0f);

    std::vector<std::unique_ptr<Triangle>> triangles = WE_EMPTY_VECTOR;

};

#endif // WE_OBJ_OBJECT_H_