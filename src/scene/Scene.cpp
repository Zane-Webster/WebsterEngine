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
}

void Scene::Destroy() {
    for (auto& item : items) {
        switch (item->type) {
            case WE::RENDERITEM_TYPE::OBJECT:
                auto object = std::static_pointer_cast<Object>(item->ptr);
                object->Destroy();
                break;
        }
    }
    Scene::Clear();
}

//=============================
// ITEMS
//=============================

void Scene::AddItem(std::shared_ptr<WE::RenderItem> item) {
    items.push_back(item);
}

void Scene::AddItems(std::vector<std::shared_ptr<WE::RenderItem>> p_items) {
    for (const auto& item : p_items) {
       Scene::AddItem(item);
    }
}

void Scene::RemoveItem(std::string p_name) {
    size_t pre_size = items.size();

    items.erase(
        std::remove_if(items.begin(), items.end(),
            [&](const std::shared_ptr<WE::RenderItem>& item) {
                return item && item->name == p_name;
            }),
        items.end()
    );

    if (pre_size == items.size()) Logger::Warn("[Scene::RemoveItem] ITEM: " + p_name + " COULD NOT BE FOUND");
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

    auto object = std::static_pointer_cast<Object>(item->ptr);
    if (!object) {
        Logger::Warn("[Scene::GetObject] OBJECT: " + p_name + " IS NOT TYPE: OBJECT");
        return nullptr;
    }

    return object;
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

        if (item->type != WE::RENDERITEM_TYPE::OBJECT) continue;

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