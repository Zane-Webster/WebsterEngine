#include "obj/Object.h"

Object::Object(std::string p_name, std::vector<std::unique_ptr<Triangle>>&& p_triangles) : name(p_name), triangles(std::move(p_triangles)) {
    ;
}

void Object::Destroy() {
    for (auto& triangle : triangles) {
        triangle->Destroy();
    }
}

void Object::Render() {
    for (auto& triangle : triangles) {
        triangle->Render();
    }
}