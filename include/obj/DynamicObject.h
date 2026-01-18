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

    void ApplyForce(glm::vec3 force);       // a force acts over time, e.g. wind pushing an object
    void ApplyImpulse(glm::vec3 impulse);   // an impulse is a jolt of force, e.g. jumping or throwing an object

    void SetVelocity(glm::vec3 velocity);

    void ProcessPhysics(double delta_time);
    void ResetPhysics();
    
    bool IsMoving();

    bool grounded = false;
    bool use_gravity = true;

private:
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 accumulated_force = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);

    float max_speed = 4.0f;
    float linear_damping = 0.98f;

    float mass = 1.0f;

    void _ApplyGravity();
};

#endif // WE_OBJ_DYNAMICOBJECT_H_