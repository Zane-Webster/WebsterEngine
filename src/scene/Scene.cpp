#include "scene/Scene.h"

Scene::Scene(std::string p_name) : name(p_name) {
    ;
}

//=============================
// BASIC
//=============================

void Scene::Clear() {
    items.clear();
    lights.clear();
    dynamic_objects.clear();
    static_objects.clear();
}

void Scene::Destroy() {
    for (auto& item : items) {
        switch (item->type) {
            case WE::RENDERITEM_TYPE::OBJECT: {
                auto object = std::static_pointer_cast<Object>(item->ptr);
                object->Destroy();
                break;
            }
            case WE::RENDERITEM_TYPE::DYNAMIC_OBJECT: {
                auto object = std::static_pointer_cast<DynamicObject>(item->ptr);
                object->Destroy();
                break;
            }
            case WE::RENDERITEM_TYPE::STATIC_OBJECT: {
                auto object = std::static_pointer_cast<StaticObject>(item->ptr);
                object->Destroy();
                break;
            }
        }
    }

    Scene::Clear();
}

//=============================
// ITEMS
//=============================

void Scene::AddItem(std::shared_ptr<WE::RenderItem> item) {
    items.push_back(item);
    if (item->type == WE::RENDERITEM_TYPE::DYNAMIC_OBJECT) dynamic_objects.push_back(static_cast<DynamicObject*>(item->ptr.get()));
    else if (item->type == WE::RENDERITEM_TYPE::STATIC_OBJECT) static_objects.push_back(static_cast<StaticObject*>(item->ptr.get()));
}

void Scene::AddItems(std::vector<std::shared_ptr<WE::RenderItem>> p_items) {
    for (const auto& item : p_items) {
       Scene::AddItem(item);
    }
}

void Scene::RemoveItem(std::string p_name) {
    const size_t pre_size = items.size();

    items.erase(
        std::remove_if(items.begin(), items.end(),
            [&](const std::shared_ptr<WE::RenderItem>& item) {
                return item && item->name == p_name;
            }),
        items.end()
    );

    auto removeRaw = [&](auto& vec) {
        vec.erase(
            std::remove_if(vec.begin(), vec.end(),
                [&](auto* ptr) {
                    return ptr && ptr->name == p_name;
                }),
            vec.end()
        );
    };

    removeRaw(dynamic_objects);
    removeRaw(static_objects);

    if (items.size() == pre_size) Logger::Warn("[Scene::RemoveItem] ITEM: " + p_name + " COULD NOT BE FOUND");
}


void Scene::RemoveItems(std::vector<std::string> names) {
    for (const auto& i_name : names) {
        Scene::RemoveItem(i_name);
    }
}

std::shared_ptr<WE::RenderItem> Scene::GetItem(std::string p_name) {
    for (const auto& item : items) {
        if (item && item->name == p_name) return item;
    }

    Logger::Warn("[Scene::GetItem] ITEM: " + p_name + " COULD NOT BE FOUND");
    return nullptr;
}

std::shared_ptr<Object> Scene::GetObject(std::string p_name) {
    std::shared_ptr<WE::RenderItem> item = Scene::GetItem(p_name);

    if (item->type != WE::RENDERITEM_TYPE::OBJECT && item->type != WE::RENDERITEM_TYPE::STATIC_OBJECT && item->type != WE::RENDERITEM_TYPE::DYNAMIC_OBJECT) {
        Logger::Warn("[Scene::GetObject] ITEM: " + p_name + " IS TYPE: " + Utils::WETypeToString(item->type));
        return nullptr;
    }

    return std::static_pointer_cast<Object>(item->ptr);
}

//=============================
// LIGHTS
//=============================

void Scene::AddLight(std::shared_ptr<WE::Light> light) {
    lights.push_back(light);
}

void Scene::RemoveLight(std::string p_name) {
    size_t pre_size = lights.size();

    lights.erase(
        std::remove_if(lights.begin(), lights.end(),
            [&](const std::shared_ptr<WE::Light>& light) {
                return light && light->name == p_name;
            }),
        lights.end()
    );

    if (pre_size == lights.size()) Logger::Warn("[Scene::RemoveLight] LIGHT: " + p_name + " COULD NOT BE FOUND");
}

//=============================
// RAYCASTING
//=============================

bool Scene::Raycast(WE::Ray ray, WE::RayHit& out_hit) {
    bool hit_any = false;
    float closest = out_hit.distance;

    for (const auto& item : items) {
        if (!item || !item->raycastable) continue;

        if (item->type != WE::RENDERITEM_TYPE::OBJECT && item->type != WE::RENDERITEM_TYPE::STATIC_OBJECT && item->type != WE::RENDERITEM_TYPE::DYNAMIC_OBJECT) continue;

        auto object = static_cast<Object*>(item->ptr.get());
        if (!object) continue;

        WE::RayHit temp = {};
        temp.distance = closest;

        if (object->Raycast(ray, temp) && temp.distance < closest) {
            closest = temp.distance;
            out_hit = temp;
            out_hit.item = item.get();
            hit_any = true;
        }
    }

    return hit_any;
}

//=============================
// PHYSICS
//=============================

bool Scene::ProcessPhysics(double delta_time) {
    bool any_moving = false;
    for (DynamicObject* dyn : dynamic_objects) {
        any_moving |= dyn->ProcessPhysics(delta_time); // (|=) bitwise or ; (a | b) = a ; once any_moving = true, will always return true 
    }
    return any_moving;
}

void Scene::ApplyPhysics(double delta_time) {
    for (DynamicObject* dyn : dynamic_objects) {
        dyn->ApplyPhysics(delta_time);
    }
}

void Scene::ProcessCollisions(double delta_time) {
    for (DynamicObject* dyn : dynamic_objects) {
        for (StaticObject* sta : static_objects) {
            // AABB check
            if (CollisionUtils::AABBIntersects(dyn->predicted_aabb, sta->GetAABB())) {
                // full collider check
                if (CollisionUtils::CollidersIntersect(*dyn->GetCollider(), *sta->GetCollider())) {
                    Logger::Debug(sta->name);
                }
            }
        }
    }
}

bool Scene::ItemIntersectsAABB(std::string p_name) {
    auto active_item = GetItem(p_name);

    auto active_object = static_cast<DynamicObject*>(active_item->ptr.get());

    if (!active_object) return false;

    for (auto& item : items) {
        if (item == active_item) continue;

        auto collidable = static_cast<Object*>(item->ptr.get());

        if (!collidable) continue;

        if (CollisionUtils::AABBIntersects(active_object->predicted_aabb, collidable->GetAABB())) {
            Logger::Debug(item->name);
            return true;
        }
    }

    return false;
}