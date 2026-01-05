#include "obj/DynamicObject.h"

void DynamicObject::ProcessPhysics() {
    DynamicObject::_ApplyGravity();

    DynamicObject::Translate(velocity);
}

void DynamicObject::_ApplyGravity() {
    velocity += resting_velocity;
}