#ifndef WE_OBJ_DYNAMICOBJECT_H_
#define WE_OBJ_DYNAMICOBJECT_H_

#include <iostream>
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "obj/Object.h"

class DynamicObject : public Object {
public:
    using Object::Object;

    // ======== SETTER ========
    void SetDynamicProperties(float mass = 2.0f, float max_speed = 4.0f, float restitution = 0.7f, float linear_damping = 0.98f, bool use_gravity = true);
    void SetVelocity(glm::vec3 velocity);

    // ======== APPLY PHYSICS ========
    void ApplyPhysics();
    void ApplyForce(glm::vec3 force);       // a force acts over time, e.g. wind pushing an object
    void ApplyImpulse(glm::vec3 impulse);   // an impulse is a jolt of force, e.g. jumping or throwing an object
    void ApplyAngularImpulse(glm::vec3 impulse, WE::CollisionManifold manifold); // an impulse at an angle based on collision manifold

    // ======== PROCESS ========
    bool ProcessPhysics(double delta_time);
    void ProcessGrounded(WE::CollisionManifold manifold);
    void ProcessManifold(WE::CollisionManifold manifold);
    void ProcessDynamicCollision(DynamicObject& other, WE::CollisionManifold manifold);

    // ======== RESET ========
    void ResetPhysics();

    // ======== UPDATE ========
    void UpdatePredictedAABB();
    void IntegrateAngular(float dt);
    
    // ======== GETTER ========
    bool IsMoving();

    // ======== BASIC PROPERTIES ========
    bool grounded = false;
    bool use_gravity = true;
    float mass = 2.0f;
    float max_speed = 4.0f;
    float restitution = 0.7f;
    float friction = 4.0f;
    float static_friction = 2.2f;
    
    float inv_mass = 0.5f;
    float linear_damping = 0.98f;
    float angular_damping = 0.99f;

    glm::vec3 predicted_position = glm::vec3(0.0f);
    WE::AABB predicted_aabb = {};
    
    glm::vec3 accumulated_force = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);

    glm::vec3 ground_normal = glm::vec3(0.0f);

    // ======== ANGULAR PROPERTIES ========
    glm::quat orientation = glm::quat(1, 0, 0, 0);
    glm::vec3 angular_velocity = glm::vec3(0.0f);
    glm::mat3 inertia_body = glm::mat3(1.0f);
    glm::mat3 inv_inertia_body = glm::mat3(1.0f);
    glm::mat3 inv_inertia_world = glm::mat3(1.0f);
    glm::vec3 accumulated_torque = glm::vec3(0.0f);
    glm::vec3 center_of_mass = glm::vec3(0.0f);

private:
    // ======== BASIC PHYSICS ========
    void _UpdateModelMatrix() override;
    void _ApplyGravity();
    void _ProcessMovement(float dt);

    // ======== DYNAMIC VS STATIC COLLISION ========
    void _CalculateDynamicStaticCorrection(WE::CollisionManifold manifold);
    void _CalculateDynamicStaticImpulse(WE::CollisionManifold manifold);

    // ======== DYNAMIC VS DYNAMIC COLLISION ========
    void _CalculateDynamicDynamicCorrection(DynamicObject& other, WE::CollisionManifold manifold, glm::vec3 normal);
    void _CalculateDynamicDynamicImpulse(DynamicObject& other, WE::CollisionManifold manifold, glm::vec3 normal);

    // ======== ANGULAR PHYSICS ========
    void _ComputeBoxInertia();
    void _UpdateInvInertiaWorld();
    void _UpdateCOM();
    glm::vec3 _GetVelocityAtPoint(glm::vec3 world_point);
};

#endif // WE_OBJ_DYNAMICOBJECT_H_