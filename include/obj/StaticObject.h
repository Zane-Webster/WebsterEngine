#ifndef WE_OBJ_STATICOBJECT_H_
#define WE_OBJ_STATICOBJECT_H_

#include <iostream>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "obj/Object.h"

class StaticObject : public Object {
public:
    using Object::Object;
    void Translate(glm::vec3 translation) override { Logger::Warn("[StaticObject::Translate] CANNOT TRANSLATE STATIC OBJECT: " + name); };
    
private:
    
};

#endif // WE_OBJ_STATICOBJECT_H_