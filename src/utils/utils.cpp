#include "utils/Utils.h"

const std::unordered_map<SDL_Scancode, glm::vec3> Utils::wasd_keyset_map = {
    { SDL_SCANCODE_W,      {  0,  0, 1 } },
    { SDL_SCANCODE_S,      {  0,  0,  -1 } },
    { SDL_SCANCODE_A,      { -1,  0,  0 } },
    { SDL_SCANCODE_D,      {  1,  0,  0 } },
    { SDL_SCANCODE_SPACE,  {  0,  1,  0 } },
    { SDL_SCANCODE_LCTRL,  {  0, -1,  0 } }
};

const std::unordered_map<SDL_Scancode, glm::vec3> Utils::arrows_keyset_map = {
    { SDL_SCANCODE_UP,     {  0,  0, -1 } },
    { SDL_SCANCODE_DOWN,   {  0,  0,  1 } },
    { SDL_SCANCODE_LEFT,   { -1,  0,  0 } },
    { SDL_SCANCODE_RIGHT,  {  1,  0,  0 } },
    { SDL_SCANCODE_SPACE,  {  0,  1,  0 } },
    { SDL_SCANCODE_LCTRL,  {  0, -1,  0 } }
};

glm::vec3 Utils::MovementFromScancode(WE::KEYSET keyset, SDL_Scancode scancode) {
    const auto& map =
        (keyset == WE::KEYSET::WASD)
            ? wasd_keyset_map
            : arrows_keyset_map;

    auto it = map.find(scancode);
    if (it != map.end()) return it->second;

    return glm::vec3(0.0f);
}

bool Utils::RayIntersectsAABB(WE::Ray ray, WE::AABB box, float& t) {
    float tmin = 0.0f;
    float tmax = FLT_MAX;

    for (int i = 0; i < 3; i++) {
        float invD = 1.0f / ray.direction[i];
        float t0 = (box.min[i] - ray.origin[i]) * invD;
        float t1 = (box.max[i] - ray.origin[i]) * invD;

        if (invD < 0.0f) std::swap(t0, t1);

        tmin = std::max(tmin, t0);
        tmax = std::min(tmax, t1);

        if (tmax <= tmin) return false;
    }

    t = tmin;
    return true;
}

bool Utils::RayIntersectsTriangle(WE::Ray ray, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float& t) {
    constexpr float EPS = 1e-7f;

    glm::vec3 e1 = v1 - v0;
    glm::vec3 e2 = v2 - v0;

    glm::vec3 h = glm::cross(ray.direction, e2);
    float a = glm::dot(e1, h);

    if (fabs(a) < EPS) return false;

    float f = 1.0f / a;
    glm::vec3 s = ray.origin - v0;
    float u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f) return false;

    glm::vec3 q = glm::cross(s, e1);
    float v = f * glm::dot(ray.direction, q);
    
    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * glm::dot(e2, q);
    return t > EPS;
}