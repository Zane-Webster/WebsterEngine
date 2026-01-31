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

    return extents.x * std::abs(axis.x) + extents.y * std::abs(axis.y) + extents.z * std::abs(axis.z);
}

float CollisionUtils::ProjectOBB(const WE::OBBShape& obb, const glm::vec3& axis) {
    return obb.half_extents.x * std::abs(glm::dot(axis, obb.axis[0])) + obb.half_extents.y * std::abs(glm::dot(axis, obb.axis[1])) + obb.half_extents.z * std::abs(glm::dot(axis, obb.axis[2]));
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
// OBB HELPER
// ==================================

glm::vec3 CollisionUtils::SupportPointOBB(const WE::OBBShape& obb, const glm::vec3& dir) {
    glm::vec3 result = obb.center;

    for (int i = 0; i < 3; i++) {
        float sign = glm::dot(dir, obb.axis[i]) > 0.0f ? 1.0f : -1.0f;
        result += obb.axis[i] * obb.half_extents[i] * sign;
    }

    return result;
}

glm::vec3 CollisionUtils::SupportFaceCenterOBB(const WE::OBBShape& obb, const glm::vec3& dir) {
    // pick the OBB axis most aligned with dir
    int best = 0;
    float bestAbs = std::abs(glm::dot(dir, obb.axis[0]));

    for (int i = 1; i < 3; i++) {
        float a = std::abs(glm::dot(dir, obb.axis[i]));
        if (a > bestAbs) { bestAbs = a; best = i; }
    }

    float s = (glm::dot(dir, obb.axis[best]) >= 0.0f) ? 1.0f : -1.0f;
    return obb.center + obb.axis[best] * (obb.half_extents[best] * s);
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
    WE::CollisionManifold m;

    glm::vec3 aabbCenter = (aabb.min + aabb.max) * 0.5f;
    glm::vec3 d = aabbCenter - obb.center;

    float minOverlap = FLT_MAX;
    glm::vec3 bestAxis;

    // =========================
    // 1. OBB axes
    // =========================
    for (int i = 0; i < 3; i++) {
        glm::vec3 axis = obb.axis[i];

        float dist = std::abs(glm::dot(d, axis));
        float r1 = ProjectOBB(obb, axis);
        float r2 = ProjectAABB(aabb, axis);

        float overlap = r1 + r2 - dist;
        if (overlap < 0.0f) return m;

        if (overlap < minOverlap) {
            minOverlap = overlap;
            bestAxis = axis;
        }
    }

    // =========================
    // 2. World axes
    // =========================
    const glm::vec3 worldAxes[3] = {
        {1,0,0}, {0,1,0}, {0,0,1}
    };

    for (int i = 0; i < 3; i++) {
        glm::vec3 axis = worldAxes[i];

        float dist = std::abs(glm::dot(d, axis));
        float r1 = ProjectOBB(obb, axis);
        float r2 = ProjectAABB(aabb, axis);

        float overlap = r1 + r2 - dist;
        if (overlap < 0.0f) return m;

        if (overlap < minOverlap) {
            minOverlap = overlap;
            bestAxis = axis;
        }
    }

    // =========================
    // Collision confirmed
    // =========================
    m.hit = true;
    m.penetration = minOverlap;

    glm::vec3 n = glm::normalize(bestAxis);
    if (glm::dot(n, d) < 0.0f)
        n = -n;

    m.normal = n;

    // ======================================================
    // 3. Build contact manifold (face contact)
    // ======================================================

    // center of OBB face opposing normal
    glm::vec3 faceCenter = SupportFaceCenterOBB(obb, -n);

    // choose face axes
    float d0 = std::abs(glm::dot(n, obb.axis[0]));
    float d1 = std::abs(glm::dot(n, obb.axis[1]));
    float d2 = std::abs(glm::dot(n, obb.axis[2]));

    glm::vec3 u, v;
    float hu, hv;

    if (d0 > d1 && d0 > d2) {
        u = obb.axis[1];
        v = obb.axis[2];
        hu = obb.half_extents.y;
        hv = obb.half_extents.z;
    }
    else if (d1 > d2) {
        u = obb.axis[0];
        v = obb.axis[2];
        hu = obb.half_extents.x;
        hv = obb.half_extents.z;
    }
    else {
        u = obb.axis[0];
        v = obb.axis[1];
        hu = obb.half_extents.x;
        hv = obb.half_extents.y;
    }

    // 4 face corners
    glm::vec3 pts[4] = {
        faceCenter + u*hu + v*hv,
        faceCenter - u*hu + v*hv,
        faceCenter + u*hu - v*hv,
        faceCenter - u*hu - v*hv
    };

    // =========================
    // 4. Clip against AABB
    // =========================
    m.contact_count = 0;

    for (int i = 0; i < 4; i++) {
        glm::vec3 p = pts[i];

        glm::vec3 clipped;
        clipped.x = glm::clamp(p.x, aabb.min.x, aabb.max.x);
        clipped.y = glm::clamp(p.y, aabb.min.y, aabb.max.y);
        clipped.z = glm::clamp(p.z, aabb.min.z, aabb.max.z);

        // must be behind plane
        float depth = glm::dot(clipped - faceCenter, n);
        if (depth <= 0.01f) {
            m.contacts[m.contact_count++] = clipped;
        }
    }

    // fallback (should rarely happen)
    if (m.contact_count == 0) {
        m.contacts[0] = faceCenter;
        m.contact_count = 1;
    }

    return m;
}



// ==================================
// SPHERE MANIFOLD
// ==================================

WE::CollisionManifold CollisionUtils::SphereSphereManifold(const WE::SphereShape& a, const WE::SphereShape& b) {
    WE::CollisionManifold m;

    glm::vec3 delta = b.center - a.center;
    float dist2 = glm::dot(delta, delta);
    float r = a.radius + b.radius;

    if (dist2 >= r * r) return m;

    float dist = std::sqrt(dist2);

    m.hit = true;

    if (dist > 0.0001f) m.normal = delta / dist;
    else m.normal = glm::vec3(0, 1, 0);

    m.penetration = r - dist;

    return m;
}

WE::CollisionManifold CollisionUtils::SphereCapsuleManifold(const WE::SphereShape& sphere, const WE::CapsuleShape& capsule) {
    WE::CollisionManifold m;

    glm::vec3 axis = capsule.tip - capsule.base;
    float len2 = glm::dot(axis, axis);

    float t = 0.0f;
    if (len2 > 0.0f) t = glm::clamp(glm::dot(sphere.center - capsule.base, axis) / len2, 0.0f, 1.0f);

    glm::vec3 closest = capsule.base + axis * t;

    glm::vec3 delta = sphere.center - closest;
    float dist2 = glm::dot(delta, delta);

    float r = sphere.radius + capsule.radius;

    if (dist2 >= r * r) return m;

    float dist = std::sqrt(dist2);

    m.hit = true;

    if (dist > 0.0001f) m.normal = glm::normalize(delta);
    else m.normal = glm::vec3(0, 1, 0);

    m.penetration = r - dist;

    return m;
}

WE::CollisionManifold CollisionUtils::SphereOBBManifold(
    const WE::SphereShape& sphere,
    const WE::OBBShape& obb)
{
    WE::CollisionManifold m;

    // vector from OBB center to sphere center
    glm::vec3 d = sphere.center - obb.center;

    // closest point on OBB to sphere center
    glm::vec3 closest = obb.center;

    for (int i = 0; i < 3; i++) {
        float dist = glm::dot(d, obb.axis[i]);
        dist = glm::clamp(dist, -obb.half_extents[i], obb.half_extents[i]);
        closest += obb.axis[i] * dist;
    }

    // vector from closest point to sphere center
    glm::vec3 diff = sphere.center - closest;
    float dist2 = glm::dot(diff, diff);

    float r = sphere.radius;

    if (dist2 > r * r)
        return m;

    float dist = std::sqrt(dist2);

    // =========================
    // Build manifold
    // =========================
    m.hit = true;

    if (dist > 0.0001f) {
        m.normal = diff / dist;
        m.penetration = r - dist;
    }
    else {
        // sphere center is inside OBB
        // choose best separating axis
        float minDist = FLT_MAX;
        glm::vec3 bestAxis;

        for (int i = 0; i < 3; i++) {
            float proj = glm::dot(d, obb.axis[i]);
            float faceDist = obb.half_extents[i] - std::abs(proj);

            if (faceDist < minDist) {
                minDist = faceDist;
                bestAxis = obb.axis[i] * (proj < 0.0f ? -1.0f : 1.0f);
            }
        }

        m.normal = bestAxis;
        m.penetration = r + minDist;

        closest = sphere.center - bestAxis * r;
    }

    // =========================
    // Contact point
    // =========================
    m.contacts[0] = closest;
    m.contact_count = 1;

    return m;
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

    if (dist2 >= rsum * rsum) return m;

    float dist = std::sqrt(dist2);

    m.hit = true;

    if (dist > 0.0001f) m.normal = glm::normalize(delta);
    else m.normal = glm::vec3(0, 1, 0);

    m.penetration = rsum - dist;

    return m;
}

WE::CollisionManifold CollisionUtils::CapsuleOBBManifold(
    const WE::CapsuleShape& capsule,
    const WE::OBBShape& obb)
{
    WE::CollisionManifold m;

    // capsule segment
    glm::vec3 ab = capsule.tip - capsule.base;
    float abLen2 = glm::dot(ab, ab);

    // closest point on capsule segment to OBB center
    float t = 0.0f;
    if (abLen2 > 0.00001f) {
        t = glm::dot(obb.center - capsule.base, ab) / abLen2;
        t = glm::clamp(t, 0.0f, 1.0f);
    }

    glm::vec3 sphereCenter = capsule.base + ab * t;

    // treat capsule as sphere at closest point
    WE::SphereShape s(capsule.radius);
    s.center = sphereCenter;

    // reuse sphere–OBB
    WE::CollisionManifold sm = SphereOBBManifold(s, obb);
    if (!sm.hit)
        return m;

    // =========================
    // build capsule manifold
    // =========================
    m.hit = true;
    m.normal = sm.normal;
    m.penetration = sm.penetration;

    // contact point on capsule surface
    glm::vec3 contact =
        sphereCenter - sm.normal * capsule.radius;

    m.contacts[0] = contact;
    m.contact_count = 1;

    return m;
}

// ==================================
// OBB MANIFOLD
// ==================================
WE::CollisionManifold CollisionUtils::OBBOBBManifold(
    const WE::OBBShape& A,
    const WE::OBBShape& B)
{
    WE::CollisionManifold m;

    glm::vec3 d = B.center - A.center;

    float minOverlap = FLT_MAX;
    glm::vec3 bestAxis;
    bool axisFromA = true;
    int bestAxisIndex = 0;

    auto testAxis = [&](const glm::vec3& axis, bool fromA, int index) {
        if (glm::length(axis) < 1e-6f)
            return true;

        glm::vec3 n = glm::normalize(axis);

        float dist = std::abs(glm::dot(d, n));
        float ra = ProjectOBB(A, n);
        float rb = ProjectOBB(B, n);

        float overlap = ra + rb - dist;
        if (overlap < 0.0f)
            return false;

        if (overlap < minOverlap) {
            minOverlap = overlap;
            bestAxis = n;
            axisFromA = fromA;
            bestAxisIndex = index;
        }

        return true;
    };

    // face axes
    for (int i = 0; i < 3; i++) {
        if (!testAxis(A.axis[i], true, i)) return m;
        if (!testAxis(B.axis[i], false, i)) return m;
    }

    // edge-edge axes
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (!testAxis(glm::cross(A.axis[i], B.axis[j]), true, -1))
                return m;

    // collision confirmed
    m.hit = true;
    m.penetration = minOverlap;

    if (glm::dot(bestAxis, d) < 0)
        bestAxis = -bestAxis;

    m.normal = bestAxis;

    // =====================================
    // CONTACT GENERATION
    // =====================================

    const WE::OBBShape& ref = axisFromA ? A : B;
    const WE::OBBShape& inc = axisFromA ? B : A;

    glm::vec3 refNormal = m.normal;
    if (!axisFromA)
        refNormal = -refNormal;

    // reference face
    int refFace = 0;
    float maxDot = -FLT_MAX;
    for (int i = 0; i < 3; i++) {
        float d = glm::dot(ref.axis[i], refNormal);
        if (d > maxDot) {
            maxDot = d;
            refFace = i;
        }
    }

    glm::vec3 refAxis = ref.axis[refFace];

    // reference face center
    glm::vec3 refCenter =
        ref.center +
        refAxis * ref.half_extents[refFace] * (glm::dot(refAxis, refNormal) > 0 ? 1.f : -1.f);

    // build reference face quad
    glm::vec3 refU = ref.axis[(refFace + 1) % 3];
    glm::vec3 refV = ref.axis[(refFace + 2) % 3];

    float hu = ref.half_extents[(refFace + 1) % 3];
    float hv = ref.half_extents[(refFace + 2) % 3];

    glm::vec3 refVerts[4] = {
        refCenter + refU*hu + refV*hv,
        refCenter - refU*hu + refV*hv,
        refCenter - refU*hu - refV*hv,
        refCenter + refU*hu - refV*hv
    };

    // incident face
    int incFace = 0;
    maxDot = -FLT_MAX;
    for (int i = 0; i < 3; i++) {
        float d = glm::dot(inc.axis[i], -refNormal);
        if (d > maxDot) {
            maxDot = d;
            incFace = i;
        }
    }

    glm::vec3 incAxis = inc.axis[incFace];
    glm::vec3 incCenter =
        inc.center +
        incAxis * inc.half_extents[incFace] * (glm::dot(incAxis, -refNormal) > 0 ? 1.f : -1.f);

    glm::vec3 incU = inc.axis[(incFace + 1) % 3];
    glm::vec3 incV = inc.axis[(incFace + 2) % 3];

    float ihu = inc.half_extents[(incFace + 1) % 3];
    float ihv = inc.half_extents[(incFace + 2) % 3];

    glm::vec3 poly[4] = {
        incCenter + incU*ihu + incV*ihv,
        incCenter - incU*ihu + incV*ihv,
        incCenter - incU*ihu - incV*ihv,
        incCenter + incU*ihu - incV*ihv
    };

    // clip incident polygon against reference face planes
    std::vector<glm::vec3> contacts;

    auto clip = [&](const glm::vec3& n, float d,
                    std::vector<glm::vec3>& pts)
    {
        std::vector<glm::vec3> out;
        for (size_t i = 0; i < pts.size(); i++) {
            glm::vec3 a = pts[i];
            glm::vec3 b = pts[(i + 1) % pts.size()];

            float da = glm::dot(n, a) - d;
            float db = glm::dot(n, b) - d;

            if (da >= 0) out.push_back(a);
            if (da * db < 0) {
                float t = da / (da - db);
                out.push_back(a + t * (b - a));
            }
        }
        pts = out;
    };

    std::vector<glm::vec3> pts(poly, poly + 4);

    clip( refU,  glm::dot(refU, refCenter) + hu, pts);
    clip(-refU, -glm::dot(refU, refCenter) + hu, pts);
    clip( refV,  glm::dot(refV, refCenter) + hv, pts);
    clip(-refV, -glm::dot(refV, refCenter) + hv, pts);

    for (glm::vec3& p : pts) {
        if (m.contact_count < 4) {
            m.contacts[m.contact_count++] = p;
        }
    }

    return m;
}