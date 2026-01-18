#ifndef WE_SCENE_SCENE_H_
#define WE_SCENE_SCENE_H_

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

#include "core/WebsterEngine.h"
#include "core/Logger.h"
#include "utils/utils.h"

#include "obj/Object.h"
#include "obj/StaticObject.h"
#include "obj/DynamicObject.h"

class Scene {
public:
    Scene(std::string name);
    
    // ======== BASIC ========
    void Clear();
    void Destroy();

    // ======== ITEMS ========
    void AddItem(std::shared_ptr<WE::RenderItem> item);
    void AddItems(std::vector<std::shared_ptr<WE::RenderItem>> items);

    void RemoveItem(std::string name);
    void RemoveItems(std::vector<std::string> names);

    std::shared_ptr<WE::RenderItem> GetItem(std::string name);
    std::shared_ptr<Object> GetObject(std::string name);

    // ======== LIGHTS ========
    void AddLight(std::shared_ptr<WE::Light> light);
    void RemoveLight(std::string name);

    // ======== RAYCASTING ========
    bool Raycast(WE::Ray ray, WE::RayHit& out_hit);

    // ======== PHYSICS ========
    bool ItemIntersectsAABB(std::string name);
    
    std::string name = WE_EMPTY_STRING;

    std::vector<std::shared_ptr<WE::RenderItem>> items = WE_EMPTY_VECTOR;
    std::vector<std::shared_ptr<WE::Light>> lights = WE_EMPTY_VECTOR;

private:

};

#endif // WE_SCENE_SCENE_H_