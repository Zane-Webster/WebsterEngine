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

    std::string name = WE_EMPTY_STRING;

private:
    std::vector<std::unique_ptr<Triangle>> triangles = WE_EMPTY_VECTOR;

};

#endif // WE_OBJ_OBJECT_H_