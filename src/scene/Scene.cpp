#include "scene/Scene.h"

Scene::Scene(std::string p_name) : name(p_name) {
    ;
}

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

void Scene::Clear() {
    items.clear();
}