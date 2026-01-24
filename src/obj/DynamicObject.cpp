#include "obj/DynamicObject.h"

void DynamicObject::ApplyForce(glm::vec3 force) {
    accumulated_force += force;
}

void DynamicObject::ApplyImpulse(glm::vec3 impulse) {
    velocity += impulse / mass;
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
    grounded = false;
}

void DynamicObject::ProcessManifold(WE::CollisionManifold manifold) {    
    if (!manifold.hit) return;

    if (manifold.normal.y > 0.7f && velocity.y < 0.0f && std::abs(velocity.y) < (0.7f * (1.0f + restitution))) {
        grounded = true;
        ground_normal = manifold.normal;

        float snap = std::min(manifold.penetration, 0.015f);
        predicted_position += manifold.normal * snap;

        velocity.y = 0.0f;
        return;
    }

    if (manifold.normal.y > 0.7f) {
        grounded = true;
        ground_normal = manifold.normal;
    }

    float correction = std::max(manifold.penetration - WE_PENETRATION_SLOP, 0.0f);

    correction *= WE_CORRECTION_PERCENT;
    correction = std::min(correction, 0.06f);

    predicted_position += manifold.normal * correction;

    float velocity_normal = glm::dot(velocity, manifold.normal);

    if (velocity_normal > 0.0f) return;

    float j = -(1.0f + restitution) * velocity_normal * mass;

    glm::vec3 impulse = j * manifold.normal;

    ApplyImpulse(impulse);
}

void DynamicObject::ProcessDynamicCollision(
    DynamicObject& other,
    WE::CollisionManifold manifold)
{
    if (!manifold.hit) return;

    glm::vec3 n = manifold.normal;

    // FORCE normal from this -> other
    glm::vec3 delta = other.GetPosition() - GetPosition();
    if (glm::dot(delta, n) < 0.0f)
        n = -n;

    glm::vec3 rv = other.velocity - velocity;
    float velAlongNormal = glm::dot(rv, n);

    if (velAlongNormal > 0.0f)
        return;

    float e = std::min(restitution, other.restitution);

    float invMassA = 1.0f / mass;
    float invMassB = 1.0f / other.mass;

    float j = -(1.0f + e) * velAlongNormal;
    j /= invMassA + invMassB;

    glm::vec3 impulse = j * n;

    velocity        -= impulse * invMassA;
    other.velocity  += impulse * invMassB;

    // positional correction
    const float percent = 0.8f;
    const float slop = 0.01f;

    glm::vec3 correction =
        std::max(manifold.penetration - slop, 0.0f)
        / (invMassA + invMassB)
        * percent
        * n;

    predicted_position        -= correction * invMassA;
    other.predicted_position += correction * invMassB;
}



bool DynamicObject::IsMoving() {
    return glm::length(velocity) > 0.001f;
}

void DynamicObject::_ApplyGravity() {
    if (!use_gravity || grounded) return;

    DynamicObject::ApplyForce(glm::vec3(0.0f, WE_GRAVITY * mass, 0.0f));
}

void DynamicObject::_ProcessMovement(float dt) {
    acceleration = accumulated_force / mass;

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