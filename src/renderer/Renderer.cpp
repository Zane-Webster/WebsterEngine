#include "renderer/Renderer.h"

Renderer::Renderer() {
    ;
}

void Renderer::Build() {
    Renderer::_MakeBatches();
}

void Renderer::AddItem(std::shared_ptr<WE::RenderItem> item) {
    unbatched_items.push_back(item);
}

void Renderer::AddItems(std::vector<std::shared_ptr<WE::RenderItem>> items) {
    for (const auto& item : items) {
       Renderer::AddItem(item);
    }
}

void Renderer::RemoveItem(std::string name) {
    for (auto batch_it = items.begin(); batch_it != items.end(); ) {
        auto& batch_items = batch_it->second;

        // remove matching items from this batch
        batch_items.erase(
            std::remove_if(
                batch_items.begin(),
                batch_items.end(),
                [&](const std::shared_ptr<WE::RenderItem>& item) {
                    return item && item->name == name;
                }
            ),
            batch_items.end()
        );

        // remove empty batch
        if (batch_items.empty()) batch_it = items.erase(batch_it);
        else batch_it++;
    }
}

void Renderer::RemoveItems(std::vector<std::string> names) {
    for (const auto& name : names) {
        Renderer::RemoveItem(name);
    }
}

void Renderer::Clear() {
    unbatched_items.clear();
    items.clear();
}

void Renderer::RenderAll() {
    for (auto& [shader, batch] : items) {
        glUseProgram(shader);

        for (auto& item : batch) {
            if (!item->active) continue;

            switch (item->type) {
                case WE::RENDERITEM_TYPE::OBJECT: {
                    auto object = std::static_pointer_cast<Object>(item->ptr);
                    object->Render();
                    break;
                }
            }
        }
    }
}

void Renderer::_MakeBatches() {
    items.clear();

    // sort by layer first, shader second
    std::sort(
        unbatched_items.begin(),
        unbatched_items.end(),
        [](const auto& a, const auto& b) {
            if (!a || !b) return a != nullptr;
            if (a->layer != b->layer)
                return a->layer < b->layer;
            return a->shader_program < b->shader_program;
        }
    );

    for (const auto& item : unbatched_items) {
        GLuint shader = item->shader_program;

        // make new batch if it doesn't exist
        if (items.empty() || items.back().first != shader) {
            items.emplace_back(shader, std::vector<std::shared_ptr<WE::RenderItem>>{});
        }

        items.back().second.push_back(item);
    }

    unbatched_items.clear();
}