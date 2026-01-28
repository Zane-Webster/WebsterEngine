#include "obj/DynamicObject.h"

void DynamicObject::SetDynamicProperties(float p_mass, float p_max_speed, float p_restitution, float p_linear_damping, bool p_use_gravity) {
    mass = p_mass;
    inv_mass = 1.0f / mass;
    max_speed = p_max_speed;
    restitution = p_restitution;
    linear_damping = p_linear_damping;
    use_gravity = p_use_gravity;
}

void DynamicObject::ApplyForce(glm::vec3 force) {
    accumulated_force += force;
}

void DynamicObject::ApplyImpulse(glm::vec3 impulse) {
    velocity += impulse * inv_mass;
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

void DynamicObject::ApplyPhysics() {
    DynamicObject::SetPosition(predicted_position);
    accumulated_force = glm::vec3(0.0f);
}

void DynamicObject::ResetPhysics() {
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    accumulated_force = glm::vec3(0.0f);
    predicted_position = DynamicObject::GetPosition();
    predicted_aabb = DynamicObject::GetAABB();
    grounded = false;
}

void DynamicObject::ProcessGrounded(WE::CollisionManifold manifold) {
    if (!manifold.hit) return;

    // ==============================
    // Calculate if object is on the ground (if manifold normal is almost flat and velocity is low)
    // ==============================
    
    if (manifold.normal.y > 0.7f && velocity.y < 0.0f && std::abs(velocity.y) < (0.3f * (1.0f + restitution))) {
        grounded = true;
        ground_normal = manifold.normal;

        float snap = std::min(manifold.penetration, 0.01f);
        predicted_position += manifold.normal * snap;
        DynamicObject::UpdatePredictedAABB();

        velocity.y = 0.0f;
    }
}

void DynamicObject::ProcessManifold(WE::CollisionManifold manifold) {
    if (!manifold.hit) return;
    
    if (manifold.normal.y > 0.7f) {
        grounded = true;
        ground_normal = manifold.normal;
    }

    // ==============================
    // Calculate correction
    // ==============================

    float correction = std::max(manifold.penetration - WE_PENETRATION_SLOP, 0.0f);

    correction *= WE_CORRECTION_PERCENT;
    correction = std::min(correction, 0.02f);

    if (!grounded) predicted_position += manifold.normal * correction;
    DynamicObject::UpdatePredictedAABB();

    // ==============================
    // Calculate impulse
    // ==============================

    float velocity_normal = glm::dot(velocity, manifold.normal);
    if (velocity_normal > 0.0f) return;

    float impulse_magnitude = -(1.0f + restitution) * velocity_normal * mass;

    glm::vec3 impulse = impulse_magnitude * manifold.normal;

    ApplyImpulse(impulse);
}

void DynamicObject::ProcessDynamicCollision(DynamicObject& other, WE::CollisionManifold manifold) {
    if (!manifold.hit) return;

    if (manifold.normal.y > 0.7f) {
        grounded = true;
        ground_normal = manifold.normal;
    }

    if (manifold.normal.y < -0.7f) {
        other.grounded = true;
        other.ground_normal = manifold.normal;
    }

    glm::vec3 normal = manifold.normal;
    glm::vec3 delta = other.predicted_position - predicted_position;

    // flip normal if < 0
    if (glm::dot(delta, normal) < 0.0f) normal = -normal;

    // ==============================
    // Calculate velocity between objects
    // ==============================

    glm::vec3 relative_velocity = other.velocity - velocity;
    float vel_along_normal = glm::dot(relative_velocity, normal);

    if (vel_along_normal > 0.0f) return;

    // ==============================
    // Calculate restitution between objects
    // ==============================

    float effective_restitution = std::min(restitution, other.restitution);

    // ==============================
    // Calculate impulse
    // ==============================

    float impulse_magnitude = -(1.0f + effective_restitution) * vel_along_normal;
    impulse_magnitude /= inv_mass + other.inv_mass;

    glm::vec3 impulse = impulse_magnitude * normal;

    DynamicObject::ApplyImpulse(-impulse);
    other.ApplyImpulse(impulse);

    // ==============================
    // Calculate correction
    // ==============================

    float correction_magnitude = std::max(manifold.penetration - WE_PENETRATION_SLOP, 0.0f) / (inv_mass + other.inv_mass) * WE_CORRECTION_PERCENT;
    glm::vec3 correction = correction_magnitude * -normal;
    glm::vec3 other_correction = -correction;

    if (grounded && correction.y < 0.0f) correction.y = 0.0f;
    if (other.grounded && other_correction.y < 0.0f) other_correction.y = 0.0f;

    predicted_position += correction * inv_mass;
    other.predicted_position += other_correction * other.inv_mass;

    DynamicObject::UpdatePredictedAABB();
    other.UpdatePredictedAABB();
}

void DynamicObject::UpdatePredictedAABB() {
    predicted_aabb = DynamicObject::GetAABB();

    glm::vec3 offset = predicted_position - DynamicObject::GetPosition();
    predicted_aabb.min += offset;
    predicted_aabb.max += offset;
}

bool DynamicObject::IsMoving() {
    return glm::length(velocity) > 0.001f;
}

void DynamicObject::_ApplyGravity() {
    if (!use_gravity || grounded) return;

    DynamicObject::ApplyForce(glm::vec3(0.0f, WE_GRAVITY * mass, 0.0f));
}

void DynamicObject::_ProcessMovement(float dt) {
    acceleration = accumulated_force * inv_mass;

    velocity += acceleration * dt;
    velocity *= linear_damping;

    if (glm::length(velocity) < WE_VELOCITY_EPSILON) velocity = glm::vec3(0.0f);

    if (glm::length(velocity) > max_speed) velocity = glm::normalize(velocity) * max_speed;

    glm::vec3 desired_move = velocity * dt;

    predicted_position = DynamicObject::GetPosition() + desired_move;

    predicted_aabb = GetAABB();
    predicted_aabb.min += desired_move;
    predicted_aabb.max += desired_move;
}