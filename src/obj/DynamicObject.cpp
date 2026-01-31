#include "obj/DynamicObject.h"

void DynamicObject::SetDynamicProperties(float p_mass, float p_max_speed, float p_restitution, float p_linear_damping, bool p_use_gravity) {
    mass = p_mass;
    inv_mass = 1.0f / mass;
    max_speed = p_max_speed;
    restitution = p_restitution;
    linear_damping = p_linear_damping;
    use_gravity = p_use_gravity;
    DynamicObject::_ComputeBoxInertia();
}

void DynamicObject::ApplyForce(glm::vec3 force) {
    accumulated_force += force;
}

void DynamicObject::ApplyImpulse(glm::vec3 impulse) {
    velocity += impulse * inv_mass;
}

void DynamicObject::ApplyAngularImpulse(glm::vec3 impulse, glm::vec3 manifold_contact_point) {
    _UpdateInvInertiaWorld();

    // world center of mass = position + rotated local center
    glm::vec3 com = *position;

    if (collider && collider->type == WE::COLLIDER_TYPE::OBB) {
        glm::mat3 R = glm::mat3_cast(orientation);
        com = *position + (R * (*center));   // <-- uses Object::center (local)
    }

    glm::vec3 r = manifold_contact_point - com;
    angular_velocity += inv_inertia_world * glm::cross(r, impulse);
}

void DynamicObject::SetVelocity(glm::vec3 p_velocity) {
    velocity = p_velocity;
}

bool DynamicObject::ProcessPhysics(double delta_time) {
    float dt = static_cast<float>(delta_time);

    DynamicObject::_ApplyGravity();
    DynamicObject::_ProcessMovement(dt);
    if (collider->type == WE::COLLIDER_TYPE::OBB) DynamicObject::Integrate(dt);

    return DynamicObject::IsMoving();
}

void DynamicObject::ApplyPhysics() {
    DynamicObject::SetPosition(predicted_position);
    DynamicObject::_UpdateModelMatrix();
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

    float impulse_magnitude = -(1.0f + restitution) * velocity_normal;
    impulse_magnitude /= inv_mass;

    glm::vec3 impulse = impulse_magnitude * manifold.normal;

    ApplyImpulse(impulse);
    if (collider->type == WE::COLLIDER_TYPE::OBB) ApplyAngularImpulse(impulse, manifold.contact_point);
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
    if (collider->type == WE::COLLIDER_TYPE::OBB) ApplyAngularImpulse(-impulse, manifold.contact_point);
    other.ApplyImpulse(impulse);
    if (other.collider->type == WE::COLLIDER_TYPE::OBB) other.ApplyAngularImpulse(impulse, manifold.contact_point);

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

void DynamicObject::Integrate(float dt) {
    // angular acceleration
    DynamicObject::_UpdateInvInertiaWorld();

    glm::vec3 angular_accel = inv_inertia_world * torque_accum;
    angular_velocity += angular_accel * dt;

    // integrate quaternion
    glm::quat wq(0.0f, angular_velocity.x, angular_velocity.y, angular_velocity.z);

    orientation += 0.5f * wq * orientation * dt;
    orientation = glm::normalize(orientation);

    torque_accum = glm::vec3(0.0f);

    angular_velocity *= angular_damping;
}


bool DynamicObject::IsMoving() {
    return glm::length(velocity) > 0.001f;
}

void DynamicObject::_UpdateModelMatrix() {
    *model_matrix = glm::mat4(1.0f);
    *model_matrix = glm::translate(*model_matrix, *position);

    if (collider && collider->type == WE::COLLIDER_TYPE::OBB) {
        *model_matrix *= glm::mat4_cast(orientation);
    }

    _UpdateCollider();
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

void DynamicObject::_ComputeBoxInertia() {
    float w = half_extents.x * 2.0f;    
    float h = half_extents.y * 2.0f;
    float d = half_extents.z * 2.0f;

    float ix = (1.0f / 12.0f) * mass * (h*h + d*d);
    float iy = (1.0f / 12.0f) * mass * (w*w + d*d);
    float iz = (1.0f / 12.0f) * mass * (w*w + h*h);

    inertia_body = glm::mat3(0.0f);
    inertia_body[0][0] = ix;
    inertia_body[1][1] = iy;
    inertia_body[2][2] = iz;

    inv_inertia_body = glm::mat3(0.0f);
    inv_inertia_body[0][0] = 1.0f / ix;
    inv_inertia_body[1][1] = 1.0f / iy;
    inv_inertia_body[2][2] = 1.0f / iz;
}

void DynamicObject::_UpdateInvInertiaWorld() {
    glm::mat3 R = glm::mat3_cast(orientation);
    inv_inertia_world = R * inv_inertia_body * glm::transpose(R);
}