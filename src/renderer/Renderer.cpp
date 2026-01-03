#include "renderer/Renderer.h"

Renderer::Renderer() {
    ;
}

void Renderer::Build() {
    // add scene items to renderer
    for (auto& [name, scene] : scenes) {
        for (auto& item : scene->items) {
            Renderer::AddItem(item);
        }
    }

    Renderer::_MakeBatches();
}

void Renderer::AddScene(std::shared_ptr<Scene> scene) {
    scenes.emplace(scene->name, scene);
}

void Renderer::RemoveScene(std::string name) {
    if (scenes.contains(name)) scenes.erase(name);
    else Logger::Warn("[Renderer::RemoveScene] SCENE: " + name + " COULD NOT BE FOUND");
}

void Renderer::AddItem(std::shared_ptr<WE::RenderItem> item) {
    unbatched_items.push_back(item);
}

void Renderer::AddItems(std::vector<std::shared_ptr<WE::RenderItem>> p_items) {
    for (const auto& item : p_items) {
       Renderer::AddItem(item);
    }
}

void Renderer::RemoveItem(std::string name) {
    bool removed = false;

    for (auto batch_it = items.begin(); batch_it != items.end(); ) {
        auto& batch_items = batch_it->items;

        const size_t pre_size = batch_items.size();

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

        if (batch_items.size() != pre_size) {
            removed = true;
        }

        // erase empty batch
        if (batch_items.empty()) {
            batch_it = items.erase(batch_it);
        } else {
            ++batch_it;
        }
    }

    if (!removed) Logger::Warn("[Renderer::RemoveItem] ITEM: " + name + " COULD NOT BE FOUND");
}

void Renderer::RemoveItems(std::vector<std::string> names) {
    for (const auto& name : names) {
        Renderer::RemoveItem(name);
    }
}

void Renderer::Clear() {
    unbatched_items.clear();
    items.clear();
    scenes.clear();
}

void Renderer::RenderAll(glm::mat4 view_projection_matrix) {
    for (auto& batch : items) {
        glUseProgram(batch.program);
        
        glUniformMatrix4fv(batch.uniforms.view_projection, 1, GL_FALSE, glm::value_ptr(view_projection_matrix));

        for (auto& item : batch.items) {
            if (!item->active) continue;

            switch (item->type) {
                case WE::RENDERITEM_TYPE::OBJECT: {
                    auto object = std::static_pointer_cast<Object>(item->ptr);
                    glUniformMatrix4fv(batch.uniforms.model, 1, GL_FALSE, glm::value_ptr(object->GetModelMatrix()));
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
        if (items.empty() || items.back().program != shader) {
            WE::RenderBatch batch = {};
            batch.program = shader;

            batch.uniforms.model =
                glGetUniformLocation(shader, "model");

            batch.uniforms.view_projection =
                glGetUniformLocation(shader, "view_projection");

            items.push_back(std::move(batch));
        }

        items.back().items.push_back(item);
    }

    unbatched_items.clear();
}