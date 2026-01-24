#include "obj/Object.h"

Object::Object(std::string p_name, WE::Material p_material, std::vector<std::unique_ptr<Triangle>>&& p_triangles, WE::COLLIDER_TYPE collider_type, glm::vec3 p_origin) : name(p_name), triangles(std::move(p_triangles)), material(p_material) {
    *origin = p_origin;
    *position = *origin;
    *origin_model_matrix = glm::translate(*origin_model_matrix, *origin);
    Object::_CalculateAABB();
    Object::_BuildCollider(collider_type);
    Object::_UpdateModelMatrix();
}

//=============================
// BASIC
//=============================

void Object::Render() {
    for (auto& triangle : triangles) {
        triangle->Render();
    }
}

void Object::Destroy() {
    for (auto& triangle : triangles) {
        triangle->Destroy();
    }
}

//=============================
// MOVEMENT
//=============================

void Object::SetPosition(glm::vec3 p_position) {
    *position = p_position;
    Object::_UpdateModelMatrix();
}

void Object::Translate(glm::vec3 translation) {
    *position += translation;
    Object::_UpdateModelMatrix();
}

void Object::ResetToOrigin() {
    *position = *origin;
    *model_matrix = *origin_model_matrix;
}

//=============================
// RAYCASTING
//=============================

bool Object::Raycast(WE::Ray ray, WE::RayHit& hit) {
    // transform ray into local space
    glm::mat4 inv_model = glm::inverse(*model_matrix);

    WE::Ray local_ray;
    local_ray.origin = glm::vec3(inv_model * glm::vec4(ray.origin, 1.0f));

    local_ray.direction = glm::normalize(glm::vec3(inv_model * glm::vec4(ray.direction, 0.0f)));

    // test against aabb
    float t_box;
    if (!Utils::RayIntersectsAABB(local_ray, aabb, t_box)) return false;

    bool hit_any = false;
    float closest = hit.distance;

    // test against triangles if aabb hit
    for (const auto& tri : triangles) {
        float t;
        if (Utils::RayIntersectsTriangle(local_ray, tri->v0, tri->v1, tri->v2, t) && t < closest) {
            closest = t;
            hit_any = true;

            // local hit pos
            glm::vec3 local_pos = local_ray.origin + local_ray.direction * t;

            // world hit pos
            hit.position = glm::vec3(*model_matrix * glm::vec4(local_pos, 1.0f));

            // local normal
            glm::vec3 local_normal = glm::normalize(glm::cross(tri->v1 - tri->v0, tri->v2 - tri->v0));

            // world normal
            hit.normal = glm::normalize(glm::mat3(glm::transpose(inv_model)) * local_normal);

            // world distance
            hit.distance = glm::length(hit.position - ray.origin);
        }
    }

    return hit_any;
}

//=============================
// COLOR
//=============================

void Object::SetColor(glm::vec3 p_color) {
    for (auto& tri : triangles) {
        tri->SetColor(p_color);
    }
}

//=============================
// GETTERS
//=============================

WE::AABB Object::GetAABB() {
    glm::vec3 corners[8] = {
        {aabb.min.x, aabb.min.y, aabb.min.z},
        {aabb.max.x, aabb.min.y, aabb.min.z},
        {aabb.min.x, aabb.max.y, aabb.min.z},
        {aabb.max.x, aabb.max.y, aabb.min.z},
        {aabb.min.x, aabb.min.y, aabb.max.z},
        {aabb.max.x, aabb.min.y, aabb.max.z},
        {aabb.min.x, aabb.max.y, aabb.max.z},
        {aabb.max.x, aabb.max.y, aabb.max.z}
    };

    WE::AABB world;
    world.min = glm::vec3(std::numeric_limits<float>::infinity());
    world.max = glm::vec3(-std::numeric_limits<float>::infinity());

    for (const glm::vec3& c : corners) {
        glm::vec3 wc =
            glm::vec3(*model_matrix * glm::vec4(c, 1.0f));

        world.min = glm::min(world.min, wc);
        world.max = glm::max(world.max, wc);
    }

    return world;
}

//=============================
// PRIVATE
//=============================

void Object::_CalculateAABB() {
    aabb.min = glm::vec3(std::numeric_limits<float>::infinity());
    aabb.max = glm::vec3(-std::numeric_limits<float>::infinity());

    for (const auto& tri : triangles) {
        aabb.min = glm::min(aabb.min, tri->v0);
        aabb.min = glm::min(aabb.min, tri->v1);
        aabb.min = glm::min(aabb.min, tri->v2);

        aabb.max = glm::max(aabb.max, tri->v0);
        aabb.max = glm::max(aabb.max, tri->v1);
        aabb.max = glm::max(aabb.max, tri->v2);
    }

    *size = aabb.max - aabb.min;
    *center = (aabb.min + aabb.max) * 0.5f;
}

void Object::_UpdateModelMatrix() {
    *model_matrix = glm::mat4(1.0f);
    *model_matrix = glm::translate(*model_matrix, *position);
    
    Object::_UpdateCollider();
}

void Object::_BuildCollider(WE::COLLIDER_TYPE type) {
    switch (type) {
        case WE::COLLIDER_TYPE::UNDECLARED:
        case WE::COLLIDER_TYPE::AABB:
            collider = std::make_unique<WE::AABBShape>(aabb);
            break;
        case WE::COLLIDER_TYPE::SPHERE: {
            float radius = 0.5f * glm::length(*size);
            collider =  std::make_unique<WE::SphereShape>(radius);
            break;
        }

        case WE::COLLIDER_TYPE::CAPSULE: {
            float radius = 0.5f * glm::max(size->x, size->z);
            float height = glm::max(size->y - 2.0f * radius, 0.0f);
            collider =  std::make_unique<WE::CapsuleShape>(radius, height);
            break;
        }
    }
}

void Object::_UpdateCollider() {
    glm::vec3 pos = Object::GetPosition();
    collider->center = pos;

    switch (collider->type) {
        case WE::COLLIDER_TYPE::SPHERE: {
            // just needs center updated
            //auto* sphere = static_cast<WE::SphereShape*>(collider.get());

            break;
        }

        case WE::COLLIDER_TYPE::CAPSULE: {
            auto* cap = static_cast<WE::CapsuleShape*>(collider.get());

            float half = cap->height * 0.5f;

            cap->base = pos - glm::vec3(0.0f, half, 0.0f);
            cap->tip  = pos + glm::vec3(0.0f, half, 0.0f);
            break;
        }

        case WE::COLLIDER_TYPE::AABB: {
            auto* box = static_cast<WE::AABBShape*>(collider.get());

            box->world_box = Object::GetAABB();

            break;
        }
    }
}