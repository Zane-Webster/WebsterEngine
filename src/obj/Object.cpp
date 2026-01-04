#include "obj/Object.h"

Object::Object(std::string p_name, WE::Material p_material, std::vector<std::unique_ptr<Triangle>>&& p_triangles) : name(p_name), triangles(std::move(p_triangles)), material(p_material) {
    Object::_CalculateAABB();
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

void Object::Translate(glm::vec3 translation) {
    *position += translation;
    *model_matrix = glm::translate(*model_matrix, translation);

    Object::_CalculateAABB();
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
}