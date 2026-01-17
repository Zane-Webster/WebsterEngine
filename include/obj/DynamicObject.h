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

    void ProcessPhysics();
    
    bool moving = true;

private:
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);
    glm::vec3 resting_velocity = glm::vec3(0.0f, WE_GRAVITY, 0.0f);
    float max_speed = 5.0f;

    float mass = 1.0f;

    void _ApplyGravity();
};

#endif // WE_OBJ_DYNAMICOBJECT_H_