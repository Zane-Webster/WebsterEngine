#ifndef WE_OBJ_DYNAMICOBJECT_H_
#define WE_OBJ_DYNAMICOBJECT_H_

#include <iostream>
#include <memory>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "obj/Object.h"

class DynamicObject : public Object {
public:
    using Object::Object;

    void SetDynamicProperties(float mass = 2.0f, float max_speed = 4.0f, float restitution = 0.7f, float linear_damping = 0.98f, bool use_gravity = true);

    void ApplyForce(glm::vec3 force);       // a force acts over time, e.g. wind pushing an object
    void ApplyImpulse(glm::vec3 impulse);   // an impulse is a jolt of force, e.g. jumping or throwing an object

    void SetVelocity(glm::vec3 velocity);

    bool ProcessPhysics(double delta_time);
    void ApplyPhysics();
    void ResetPhysics();

    void ProcessGrounded(WE::CollisionManifold manifold);

    void ProcessManifold(WE::CollisionManifold manifold);
    void ProcessDynamicCollision(DynamicObject& other, WE::CollisionManifold manifold);

    void UpdatePredictedAABB();
    
    bool IsMoving();

    WE::AABB predicted_aabb = {};

    bool grounded = false;
    bool use_gravity = true;

    glm::vec3 predicted_position = glm::vec3(0.0f);

    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 accumulated_force = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);

    glm::vec3 ground_normal = glm::vec3(0.0f);

    float mass = 2.0f;
    float inv_mass = 0.5f;
    float max_speed = 4.0f;
    float restitution = 0.7f;
    float linear_damping = 0.98f;

private:

    void _ApplyGravity();
    void _ProcessMovement(float dt);
};

#endif // WE_OBJ_DYNAMICOBJECT_H_