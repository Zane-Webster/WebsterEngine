#include "utils/CollisionUtils.h"

bool CollisionUtils::AABBIntersects(WE::AABB a, WE::AABB b) {
    return
        a.min.x <= b.max.x && a.max.x >= b.min.x &&
        a.min.y <= b.max.y && a.max.y >= b.min.y &&
        a.min.z <= b.max.z && a.max.z >= b.min.z;
}

bool CollisionUtils::CollidersIntersect(const WE::ColliderShape& a, const WE::ColliderShape& b) {
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

WE::CollisionManifold CollisionUtils::CollidersManifold(const WE::ColliderShape& a, const WE::ColliderShape& b) {
    using T = WE::COLLIDER_TYPE;

    // ensure symmetric handling
    if (a.type > b.type) {
        auto m = CollisionUtils::CollidersManifold(b, a);
        m.normal = -m.normal;
        return m;
    }

    switch (a.type) {
        case T::AABB: {
            const auto& boxA = static_cast<const WE::AABBShape&>(a);

            switch (b.type) {
                case T::AABB: {
                    const auto& boxB = static_cast<const WE::AABBShape&>(b);
                    return CollisionUtils::AABBAABBManifold(boxA.world_box, boxB.world_box);
                }

                case T::SPHERE: {
                    const auto& sphere = static_cast<const WE::SphereShape&>(b);
                    return CollisionUtils::SphereAABBManifold(sphere, boxA.world_box);
                }

                case T::CAPSULE: {
                    const auto& capsule = static_cast<const WE::CapsuleShape&>(b);
                    return CollisionUtils::CapsuleAABBManifold(capsule, boxA.world_box);
                }
                case T::OBB: {
                    const auto& obbB = static_cast<const WE::OBBShape&>(b);
                    return CollisionUtils::OBBAABBManifold(obbB, boxA.world_box);
                }
            }
            break;
        }

        case T::SPHERE: {
            const auto& sphereA = static_cast<const WE::SphereShape&>(a);

            switch (b.type) {
                case T::SPHERE: {
                    const auto& sphereB = static_cast<const WE::SphereShape&>(b);
                    return CollisionUtils::SphereSphereManifold(sphereA, sphereB);
                }

                case T::CAPSULE: {
                    const auto& capsule = static_cast<const WE::CapsuleShape&>(b);
                    return CollisionUtils::SphereCapsuleManifold(sphereA, capsule);
                }
                case T::OBB: {
                    const auto& obbB = static_cast<const WE::OBBShape&>(b);
                    return CollisionUtils::SphereOBBManifold(sphereA, obbB);
                }
            }
            break;
        }

        case T::CAPSULE: {
            const auto& capA = static_cast<const WE::CapsuleShape&>(a);

            switch (b.type) {
                case T::CAPSULE: {
                    const auto& capB = static_cast<const WE::CapsuleShape&>(b);
                    return CollisionUtils::CapsuleCapsuleManifold(capA, capB);
                }
                case T::OBB: {
                    const auto& obbB = static_cast<const WE::OBBShape&>(b);
                    return CollisionUtils::CapsuleOBBManifold(capA, obbB);
                }
            }

            break;
        }

        case T::OBB: {
            const auto& obbA = static_cast<const WE::OBBShape&>(a);

            if (b.type == T::OBB) {
                const auto& obbB = static_cast<const WE::OBBShape&>(b);
                return CollisionUtils::OBBOBBManifold(obbA, obbB);
            }

            break;
        }
    }

    return {};
}

float CollisionUtils::ProjectAABB(const WE::AABB& aabb, const glm::vec3& axis) {
    glm::vec3 extents = (aabb.max - aabb.min) * 0.5f;

    return
        extents.x * std::abs(axis.x) +
        extents.y * std::abs(axis.y) +
        extents.z * std::abs(axis.z);
}

float CollisionUtils::ProjectOBB(const WE::OBBShape& obb, const glm::vec3& axis) {
    return
        obb.half_extents.x * std::abs(glm::dot(axis, obb.axis[0])) +
        obb.half_extents.y * std::abs(glm::dot(axis, obb.axis[1])) +
        obb.half_extents.z * std::abs(glm::dot(axis, obb.axis[2]));
}

// ==================================
// INTERSECTS
// ==================================

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

// ==================================
// AABB MANIFOLD
// ==================================

WE::CollisionManifold CollisionUtils::AABBAABBManifold(const WE::AABB& a, const WE::AABB& b) {
    WE::CollisionManifold m;

    float xOverlap = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    float yOverlap = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    float zOverlap = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);

    if (xOverlap < -WE_CONTACT_EPSILON || yOverlap < -WE_CONTACT_EPSILON || zOverlap < -WE_CONTACT_EPSILON) return m;
    
    glm::vec3 centerA = (a.min + a.max) * 0.5f;
    glm::vec3 centerB = (b.min + b.max) * 0.5f;

    m.hit = true;

    if (xOverlap < yOverlap && xOverlap < zOverlap) {
        m.penetration = xOverlap;
        m.normal = (centerA.x < centerB.x)
            ? glm::vec3(-1, 0, 0)
            : glm::vec3( 1, 0, 0);
    }
    else if (yOverlap < zOverlap) {
        m.penetration = yOverlap;
        m.normal = (centerA.y < centerB.y)
            ? glm::vec3(0, -1, 0)
            : glm::vec3(0,  1, 0);
    }
    else {
        m.penetration = zOverlap;
        m.normal = (centerA.z < centerB.z)
            ? glm::vec3(0, 0, -1)
            : glm::vec3(0, 0,  1);
    }

    return m;
}

WE::CollisionManifold CollisionUtils::SphereAABBManifold(const WE::SphereShape& sphere, const WE::AABB& box) {
    WE::CollisionManifold m;

    glm::vec3 closest;
    closest.x = glm::clamp(sphere.center.x, box.min.x, box.max.x);
    closest.y = glm::clamp(sphere.center.y, box.min.y, box.max.y);
    closest.z = glm::clamp(sphere.center.z, box.min.z, box.max.z);

    glm::vec3 delta = sphere.center - closest;
    float dist2 = glm::dot(delta, delta);

    if (dist2 >= sphere.radius * sphere.radius)
        return m;

    float dist = std::sqrt(dist2);

    m.hit = true;

    if (dist > 0.0001f)
        m.normal = -glm::normalize(delta);
    else
        m.normal = glm::vec3(0, 1, 0);

    m.penetration = sphere.radius - dist;

    return m;
}

WE::CollisionManifold CollisionUtils::CapsuleAABBManifold(const WE::CapsuleShape& capsule, const WE::AABB& box) {
    WE::CollisionManifold m;

    glm::vec3 axis = capsule.tip - capsule.base;
    float len2 = glm::dot(axis, axis);

    glm::vec3 closestBox;
    glm::vec3 closestAxis;

    // sample closest point on capsule segment
    glm::vec3 mid = (capsule.base + capsule.tip) * 0.5f;

    closestBox.x = glm::clamp(mid.x, box.min.x, box.max.x);
    closestBox.y = glm::clamp(mid.y, box.min.y, box.max.y);
    closestBox.z = glm::clamp(mid.z, box.min.z, box.max.z);

    float t = 0.0f;
    if (len2 > 0.0f)
        t = glm::clamp(glm::dot(closestBox - capsule.base, axis) / len2, 0.0f, 1.0f);

    closestAxis = capsule.base + axis * t;

    glm::vec3 delta = closestAxis - closestBox;
    float dist2 = glm::dot(delta, delta);

    if (dist2 >= capsule.radius * capsule.radius)
        return m;

    float dist = std::sqrt(dist2);

    m.hit = true;

    if (dist > 0.0001f)
        m.normal = glm::normalize(delta);
    else
        m.normal = glm::vec3(0, 1, 0);

    m.penetration = capsule.radius - dist;

    return m;
}

WE::CollisionManifold CollisionUtils::OBBAABBManifold(const WE::OBBShape& obb, const WE::AABB& aabb) {
    return {};
}

// ==================================
// SPHERE MANIFOLD
// ==================================

WE::CollisionManifold CollisionUtils::SphereSphereManifold(const WE::SphereShape& a, const WE::SphereShape& b) {
    WE::CollisionManifold m;

    glm::vec3 delta = b.center - a.center;
    float dist2 = glm::dot(delta, delta);
    float r = a.radius + b.radius;

    if (dist2 >= r * r)
        return m;

    float dist = std::sqrt(dist2);

    m.hit = true;

    if (dist > 0.0001f)
        m.normal = delta / dist;
    else
        m.normal = glm::vec3(0, 1, 0);

    m.penetration = r - dist;

    return m;
}

WE::CollisionManifold CollisionUtils::SphereCapsuleManifold(const WE::SphereShape& sphere, const WE::CapsuleShape& capsule) {
    WE::CollisionManifold m;

    glm::vec3 axis = capsule.tip - capsule.base;
    float len2 = glm::dot(axis, axis);

    float t = 0.0f;
    if (len2 > 0.0f)
        t = glm::clamp(glm::dot(sphere.center - capsule.base, axis) / len2, 0.0f, 1.0f);

    glm::vec3 closest = capsule.base + axis * t;

    glm::vec3 delta = sphere.center - closest;
    float dist2 = glm::dot(delta, delta);

    float r = sphere.radius + capsule.radius;

    if (dist2 >= r * r)
        return m;

    float dist = std::sqrt(dist2);

    m.hit = true;

    if (dist > 0.0001f)
        m.normal = glm::normalize(delta);
    else
        m.normal = glm::vec3(0, 1, 0);

    m.penetration = r - dist;

    return m;
}

WE::CollisionManifold CollisionUtils::SphereOBBManifold(const WE::SphereShape& sphere, const WE::OBBShape& obb) {
    return {};
}

// ==================================
// CAPSULE MANIFOLD
// ==================================

WE::CollisionManifold CollisionUtils::CapsuleCapsuleManifold(const WE::CapsuleShape& a, const WE::CapsuleShape& b) {
    WE::CollisionManifold m;

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

            if (denom != 0.0f)
                s = glm::clamp((b1 * f1 - c * e1) / denom, 0.0f, 1.0f);
            else
                s = 0.0f;

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

    glm::vec3 delta = c2 - c1;
    float dist2 = glm::dot(delta, delta);

    float rsum = a.radius + b.radius;

    if (dist2 >= rsum * rsum)
        return m;

    float dist = std::sqrt(dist2);

    m.hit = true;

    if (dist > 0.0001f)
        m.normal = glm::normalize(delta);
    else
        m.normal = glm::vec3(0, 1, 0);

    m.penetration = rsum - dist;

    return m;
}

WE::CollisionManifold CollisionUtils::CapsuleOBBManifold(const WE::CapsuleShape& capsule, const WE::OBBShape& obb) {
    return {};
}

// ==================================
// OBB MANIFOLD
// ==================================
WE::CollisionManifold CollisionUtils::OBBOBBManifold(const WE::OBBShape& a, const WE::OBBShape& b) {
    return {};
}