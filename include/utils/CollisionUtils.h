#ifndef WE_UTILS_COLLISIONUTILS_H_
#define WE_UTILS_COLLISIONUTILS_H_

#include <iostream>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "glm/glm.hpp"

class CollisionUtils {
public:
    static bool AABBIntersects(WE::AABB aabb1, WE::AABB aabb2);
    static bool CollidersIntersect(const WE::ColliderShape& a, const WE::ColliderShape& b);

    static WE::CollisionManifold CollidersManifold(const WE::ColliderShape& a, const WE::ColliderShape& b);
    
private:
    static bool SphereAABBIntersect(const WE::SphereShape&, const WE::AABB&);
    static bool CapsuleAABBIntersect(const WE::CapsuleShape&, const WE::AABB&);
    static bool SphereSphereIntersect(const WE::SphereShape&, const WE::SphereShape&);
    static bool SphereCapsuleIntersect(const WE::SphereShape&, const WE::CapsuleShape&);
    static bool CapsuleCapsuleIntersect(const WE::CapsuleShape&, const WE::CapsuleShape&);

    static WE::CollisionManifold AABBAABBManifold(const WE::AABB&, const WE::AABB&);
    static WE::CollisionManifold SphereAABBManifold(const WE::SphereShape&, const WE::AABB&);
    static WE::CollisionManifold CapsuleAABBManifold(const WE::CapsuleShape&, const WE::AABB&);
    static WE::CollisionManifold SphereSphereManifold(const WE::SphereShape&, const WE::SphereShape&);
    static WE::CollisionManifold SphereCapsuleManifold(const WE::SphereShape&, const WE::CapsuleShape&);
    static WE::CollisionManifold CapsuleCapsuleManifold(const WE::CapsuleShape&, const WE::CapsuleShape&);
};

#endif // WE_UTILS_COLLISIONUTILS_H_