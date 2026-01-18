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

void DynamicObject::ProcessPhysics(double delta_time) {
    DynamicObject::_ApplyGravity();

    float dt = static_cast<float>(delta_time);

    acceleration = accumulated_force / mass;

    velocity += acceleration * dt;
    velocity *= linear_damping;

    if (glm::length(velocity) > max_speed) velocity = glm::normalize(velocity) * max_speed;

    glm::vec3 desired_move = velocity * dt;

    predicted_aabb = GetAABB();
    predicted_aabb.min += desired_move;
    predicted_aabb.max += desired_move;
}

void DynamicObject::ApplyPhysics(double delta_time) {
    float dt = static_cast<float>(delta_time);
    
    DynamicObject::Translate(velocity * dt);
    accumulated_force = glm::vec3(0.0f);
}

void DynamicObject::ResetPhysics() {
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    accumulated_force = glm::vec3(0.0f);
}

bool DynamicObject::IsMoving() {
    return glm::length(velocity) > 0.001f;
}

void DynamicObject::_ApplyGravity() {
    if (use_gravity && !grounded) DynamicObject::ApplyForce(glm::vec3(0.0f, WE_GRAVITY * mass, 0.0f));
}