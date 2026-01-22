#include "utils/CollisionUtils.h"

bool CollisionUtils::AABBIntersects(WE::AABB a, WE::AABB b) {
    return
        a.min.x <= b.max.x && a.max.x >= b.min.x &&
        a.min.y <= b.max.y && a.max.y >= b.min.y &&
        a.min.z <= b.max.z && a.max.z >= b.min.z;
}

bool CollisionUtils::CollidersIntersect(WE::ColliderShape a, WE::ColliderShape b) {
    using T = WE::COLLIDER_TYPE;

    // ensure symmetric handling
    if (a.type > b.type) return CollidersIntersect(b, a);

    switch (a.type) {
        case T::AABB: {
            const auto& boxA = static_cast<const WE::AABBShape&>(a);

            switch (b.type) {
                case T::AABB: {
                    const auto& boxB = static_cast<const WE::AABBShape&>(b);
                    return CollisionUtils::AABBIntersects(boxA.world_box, boxB.world_box);
                }

                case T::SPHERE: {
                    const auto& sphere = static_cast<const WE::SphereShape&>(b);
                    return CollisionUtils::SphereAABBIntersect(sphere, boxA.world_box);
                }

                case T::CAPSULE: {
                    const auto& capsule = static_cast<const WE::CapsuleShape&>(b);
                    return CollisionUtils::CapsuleAABBIntersect(capsule, boxA.world_box);
                }
            }
            break;
        }

        case T::SPHERE: {
            const auto& sphereA = static_cast<const WE::SphereShape&>(a);

            switch (b.type) {
                case T::SPHERE: {
                    const auto& sphereB = static_cast<const WE::SphereShape&>(b);
                    return CollisionUtils::SphereSphereIntersect(sphereA, sphereB);
                }

                case T::CAPSULE: {
                    const auto& capsule = static_cast<const WE::CapsuleShape&>(b);
                    return CollisionUtils::SphereCapsuleIntersect(sphereA, capsule);
                }
            }
            break;
        }

        case T::CAPSULE: {
            const auto& capA = static_cast<const WE::CapsuleShape&>(a);

            if (b.type == T::CAPSULE) {
                const auto& capB = static_cast<const WE::CapsuleShape&>(b);
                return CollisionUtils::CapsuleCapsuleIntersect(capA, capB);
            }
            break;
        }
    }

    return false;
}

bool CollisionUtils::SphereAABBIntersect(const WE::SphereShape& sphere, const WE::AABB& box) {
    glm::vec3 closest;

    closest.x = glm::clamp(sphere.center.x, box.min.x, box.max.x);
    closest.y = glm::clamp(sphere.center.y, box.min.y, box.max.y);
    closest.z = glm::clamp(sphere.center.z, box.min.z, box.max.z);

    glm::vec3 delta = sphere.center - closest;
    return glm::dot(delta, delta) <= sphere.radius * sphere.radius;
}

bool CollisionUtils::CapsuleAABBIntersect(const WE::CapsuleShape& capsule, const WE::AABB& box) {
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

bool CollisionUtils::SphereSphereIntersect(const WE::SphereShape& a, const WE::SphereShape& b) {
    glm::vec3 delta = a.center - b.center;
    float r = a.radius + b.radius;

    return glm::dot(delta, delta) <= r * r;
}

bool CollisionUtils::SphereCapsuleIntersect(const WE::SphereShape& sphere, const WE::CapsuleShape& capsule) {
    glm::vec3 axis = capsule.tip - capsule.base;
    float len2 = glm::dot(axis, axis);

    float t = 0.0f;
    if (len2 > 0.0f) t = glm::clamp(glm::dot(sphere.center - capsule.base, axis) / len2, 0.0f, 1.0f);

    glm::vec3 closest = capsule.base + axis * t;

    glm::vec3 delta = sphere.center - closest;
    float r = sphere.radius + capsule.radius;

    return glm::dot(delta, delta) <= r * r;
}

// don't even ask i have no idea. this collision should rarely happen anyways
bool CollisionUtils::CapsuleCapsuleIntersect(const WE::CapsuleShape& a, const WE::CapsuleShape& b) {
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