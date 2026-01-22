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

std::string Utils::WETypeToString(WE::RENDERITEM_TYPE type) {
    switch (type) {
        case WE::RENDERITEM_TYPE::OBJECT:
            return "WE::RENDERITEM_TYPE::OBJECT";
            break;
        case WE::RENDERITEM_TYPE::STATIC_OBJECT:
            return "WE::RENDERITEM_TYPE::STATIC_OBJECT";
            break;
        case WE::RENDERITEM_TYPE::DYNAMIC_OBJECT:
            return "WE::RENDERITEM_TYPE::DYNAMIC_OBJECT";
            break;
        case WE::RENDERITEM_TYPE::SKYBOX:
            return "WE::RENDERITEM_TYPE::SKYBOX";
            break;
        case WE::RENDERITEM_TYPE::TEXTURE:
            return "WE::RENDERITEM_TYPE::TEXTURE";
            break;
        case WE::RENDERITEM_TYPE::TEXT:
            return "WE::RENDERITEM_TYPE::TEXT";
            break;
        default:
            return "UNKNOWN";
            break;
    }
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

bool Utils::AABBIntersects(WE::AABB a, WE::AABB b) {
    return
        a.min.x <= b.max.x && a.max.x >= b.min.x &&
        a.min.y <= b.max.y && a.max.y >= b.min.y &&
        a.min.z <= b.max.z && a.max.z >= b.min.z;
}

bool Utils::CollidersIntersect(WE::ColliderShape a, WE::ColliderShape b) {
    using T = WE::COLLIDER_TYPE;

    // ensure symmetric handling
    if (a.type > b.type) return CollidersIntersect(b, a);

    switch (a.type) {
        case T::AABB: {
            const auto& boxA = static_cast<const WE::AABBShape&>(a);

            switch (b.type) {
                case T::AABB: {
                    const auto& boxB = static_cast<const WE::AABBShape&>(b);
                    return Utils::AABBIntersects(boxA.world_box, boxB.world_box);
                }

                case T::SPHERE: {
                    const auto& sphere = static_cast<const WE::SphereShape&>(b);
                    return Utils::SphereAABBIntersect(sphere, boxA.world_box);
                }

                case T::CAPSULE: {
                    const auto& capsule = static_cast<const WE::CapsuleShape&>(b);
                    return Utils::CapsuleAABBIntersect(capsule, boxA.world_box);
                }
            }
            break;
        }

        case T::SPHERE: {
            const auto& sphereA = static_cast<const WE::SphereShape&>(a);

            switch (b.type) {
                case T::SPHERE: {
                    const auto& sphereB = static_cast<const WE::SphereShape&>(b);
                    return Utils::SphereSphereIntersect(sphereA, sphereB);
                }

                case T::CAPSULE: {
                    const auto& capsule = static_cast<const WE::CapsuleShape&>(b);
                    return Utils::SphereCapsuleIntersect(sphereA, capsule);
                }
            }
            break;
        }

        case T::CAPSULE: {
            const auto& capA = static_cast<const WE::CapsuleShape&>(a);

            if (b.type == T::CAPSULE) {
                const auto& capB = static_cast<const WE::CapsuleShape&>(b);
                return Utils::CapsuleCapsuleIntersect(capA, capB);
            }
            break;
        }
    }

    return false;
}

bool Utils::SphereAABBIntersect(const WE::SphereShape& sphere, const WE::AABB& box) {
    glm::vec3 closest;

    closest.x = glm::clamp(sphere.center.x, box.min.x, box.max.x);
    closest.y = glm::clamp(sphere.center.y, box.min.y, box.max.y);
    closest.z = glm::clamp(sphere.center.z, box.min.z, box.max.z);

    glm::vec3 delta = sphere.center - closest;
    return glm::dot(delta, delta) <= sphere.radius * sphere.radius;
}

bool Utils::CapsuleAABBIntersect(const WE::CapsuleShape& capsule, const WE::AABB& box) {
    // clamp capsule segment midpoint into box
    glm::vec3 mid = (capsule.base + capsule.tip) * 0.5f;

    glm::vec3 closest;
    closest.x = glm::clamp(mid.x, box.min.x, box.max.x);
    closest.y = glm::clamp(mid.y, box.min.y, box.max.y);
    closest.z = glm::clamp(mid.z, box.min.z, box.max.z);

    // closest point on capsule axis
    glm::vec3 axis = capsule.tip - capsule.base;
    float len2 = glm::dot(axis, axis);

    float t = 0.0f;
    if (len2 > 0.0f) t = glm::clamp(glm::dot(closest - capsule.base, axis) / len2, 0.0f, 1.0f);

    glm::vec3 closest_axis = capsule.base + axis * t;

    glm::vec3 delta = closest - closest_axis;

    return glm::dot(delta, delta) <= capsule.radius * capsule.radius;
}

bool Utils::SphereSphereIntersect(const WE::SphereShape& a, const WE::SphereShape& b) {
    glm::vec3 delta = a.center - b.center;
    float r = a.radius + b.radius;

    return glm::dot(delta, delta) <= r * r;
}

bool Utils::SphereCapsuleIntersect(const WE::SphereShape& sphere, const WE::CapsuleShape& capsule) {
    glm::vec3 axis = capsule.tip - capsule.base;
    float len2 = glm::dot(axis, axis);

    float t = 0.0f;
    if (len2 > 0.0f) t = glm::clamp(glm::dot(sphere.center - capsule.base, axis) / len2, 0.0f, 1.0f);

    glm::vec3 closest = capsule.base + axis * t;

    glm::vec3 delta = sphere.center - closest;
    float r = sphere.radius + capsule.radius;

    return glm::dot(delta, delta) <= r * r;
}

bool Utils::CapsuleCapsuleIntersect(const WE::CapsuleShape& a, const WE::CapsuleShape& b) {
    glm::vec3 d1 = a.tip - a.base;
    glm::vec3 d2 = b.tip - b.base;
    glm::vec3 r  = a.base - b.base;

    float a1 = glm::dot(d1, d1);
    float e1 = glm::dot(d2, d2);
    float f1 = glm::dot(d2, r);

    float s, t;

    if (a1 <= 1e-6f && e1 <= 1e-6f) s = t = 0.0f;
    else if (a1 <= 1e-6f) {
        s = 0.0f;
        t = glm::clamp(f1 / e1, 0.0f, 1.0f);
    } else {
        float c = glm::dot(d1, r);

        if (e1 <= 1e-6f) {
            t = 0.0f;
            s = glm::clamp(-c / a1, 0.0f, 1.0f);
        } else {
            float b1 = glm::dot(d1, d2);
            float denom = a1 * e1 - b1 * b1;

            if (denom != 0.0f) s = glm::clamp((b1 * f1 - c * e1) / denom, 0.0f, 1.0f);
            else s = 0.0f;

            t = (b1 * s + f1) / e1;

            if (t < 0.0f) {
                t = 0.0f;
                s = glm::clamp(-c / a1, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = glm::clamp((b1 - c) / a1, 0.0f, 1.0f);
            }
        }
    }

    glm::vec3 c1 = a.base + d1 * s;
    glm::vec3 c2 = b.base + d2 * t;

    glm::vec3 delta = c1 - c2;
    float rsum = a.radius + b.radius;

    return glm::dot(delta, delta) <= rsum * rsum;
}