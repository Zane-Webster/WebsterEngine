#include "obj/DynamicObject.h"

void DynamicObject::ApplyForce(glm::vec3 force) {
    accumulated_force += force;
}

void DynamicObject::ApplyImpulse(glm::vec3 impulse) {
    velocity = impulse / mass;
}

void DynamicObject::SetVelocity(glm::vec3 p_velocity) {
    velocity = p_velocity;
}

bool DynamicObject::ProcessPhysics(double delta_time) {
    float dt = static_cast<float>(delta_time);

    DynamicObject::_ApplyGravity();
    DynamicObject::_ProcessMovement(dt);

    return DynamicObject::IsMoving();
}

void DynamicObject::ApplyPhysics(double delta_time) {
    float dt = static_cast<float>(delta_time);
    
    DynamicObject::SetPosition(predicted_position);
    accumulated_force = glm::vec3(0.0f);
}

void DynamicObject::ResetPhysics() {
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    accumulated_force = glm::vec3(0.0f);
    grounded = false;
}

void DynamicObject::ProcessManifold(WE::CollisionManifold manifold) {
    if (!manifold.hit) return;

    Logger::Debug(manifold.normal);
    Logger::Debug(manifold.penetration);

    if (manifold.normal.y > 0.7f) {
        grounded = true;
        velocity.y = 0.0f;

        // exact ground snap
        predicted_position += manifold.normal * manifold.penetration;
    } else {
        // side collisions still use slop
        float correction = std::max(manifold.penetration - WE_PENETRATION_SLOP, 0.0f);
        predicted_position += manifold.normal * correction * WE_CORRECTION_PERCENT;
    }

    float velocity_normal = glm::dot(velocity, manifold.normal);

    if (velocity_normal < 0.0f) velocity -= manifold.normal * velocity_normal;

    if (manifold.normal.y > 0.7f) grounded = true;
}

bool DynamicObject::IsMoving() {
    return glm::length(velocity) > 0.001f;
}

void DynamicObject::_ApplyGravity() {
    if (!use_gravity) return;

    float gravityScale = grounded ? 0.2f : 1.0f;

    DynamicObject::ApplyForce(glm::vec3(0.0f, WE_GRAVITY * mass * gravityScale, 0.0f));
}

void DynamicObject::_ProcessMovement(float dt) {
    acceleration = accumulated_force / mass;

    velocity += acceleration * dt;
    velocity *= linear_damping;

    if (glm::length(velocity) > max_speed) velocity = glm::normalize(velocity) * max_speed;

    glm::vec3 desired_move = velocity * dt;

    if (desired_move.y < -0.1f) desired_move.y = -0.1f;

    predicted_position = DynamicObject::GetPosition() + desired_move;

    predicted_aabb = GetAABB();
    predicted_aabb.min += desired_move;
    predicted_aabb.max += desired_move;
}