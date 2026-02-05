#include "renderer/Renderer.h"

Renderer::Renderer(Uint16 p_window_width, Uint16 p_window_height) : window_width(p_window_width), window_height(p_window_height) {
    shadow_resolution = WE_LAUNCH_SHADOW_QUALITY;
}

//=============================
// BASIC
//=============================

void Renderer::Build(GLuint p_shadow_depth_shader) {
    // add scene items to renderer
    for (auto& [name, scene] : scenes) {
        for (auto& item : scene->items) {
            Renderer::AddItem(item);
        }
        for (auto& light : scene->lights) {
            Renderer::AddLight(light);
        }
    }

    Renderer::_MakeBatches();

    shadow_depth_shader = p_shadow_depth_shader;
    Renderer::_BuildShadows();

    Renderer::_ComputeLightSpaceMatrix();
}

void Renderer::RenderAll(glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::mat4 view_projection_matrix, glm::vec3 camera_pos) {
    // =============================
    // SHADOW PASS
    // =============================
    Renderer::_RenderShadowPass();

    // restore viewport after shadow pass
    glViewport(0, 0, window_width, window_height);

    // =============================
    // MAIN RENDER PASS
    // =============================
    for (auto& batch : items) {
        glUseProgram(batch.program);
        
        glUniform1i(batch.uniforms.diffuse, 0);

        if (batch.uniforms.shadow_map != -1) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, shadow_depth_texture);
            glUniform1i(batch.uniforms.shadow_map, 1);
        }

        glUniformMatrix4fv(batch.uniforms.light_space, 1, GL_FALSE, glm::value_ptr(light_space_matrix));

        for (auto& light : lights) {
            glUniform3fv(batch.uniforms.camera_pos, 1, glm::value_ptr(camera_pos));
            glUniform3fv(batch.uniforms.light_dir, 1, glm::value_ptr(light->direction));
            glUniform3fv(batch.uniforms.light_color, 1, glm::value_ptr(light->color));
        }

        for (auto& item : batch.items) {
            if (!item->active) continue;

            switch (item->type) {
                case WE::RENDERITEM_TYPE::OBJECT:
                case WE::RENDERITEM_TYPE::STATIC_OBJECT:
                case WE::RENDERITEM_TYPE::DYNAMIC_OBJECT: {
                    auto object = std::static_pointer_cast<Object>(item->ptr);

                    glUniformMatrix4fv(batch.uniforms.view_projection, 1, GL_FALSE, glm::value_ptr(view_projection_matrix));
                    glUniformMatrix4fv(batch.uniforms.model, 1, GL_FALSE, glm::value_ptr(object->GetModelMatrix()));

                    glUniform1f(batch.uniforms.ambient_strength, object->material.ambient_strength);
                    glUniform1f(batch.uniforms.specular_strength, object->material.specular_strength);
                    glUniform1f(batch.uniforms.shininess, object->material.shininess);

                    if (object->material.diffuse) object->material.diffuse->Bind(0);
                    else basic_texture->Bind(0);

                    object->Render();
                    break;
                }
                case WE::RENDERITEM_TYPE::SKYBOX: {
                    auto sky = std::static_pointer_cast<Skybox>(item->ptr);

                    glm::mat4 sky_view = glm::mat4(glm::mat3(view_matrix));
                    glm::mat4 sky_vp = projection_matrix * sky_view;

                    glUniformMatrix4fv(batch.uniforms.view_projection, 1, GL_FALSE, glm::value_ptr(sky_vp) );

                    glDepthFunc(GL_LEQUAL);
                    sky->Render();
                    glDepthFunc(GL_LESS);
                    break;
                }

            }
        }
    }
}

void Renderer::Clear() {
    unbatched_items.clear();
    items.clear();
    scenes.clear();
    lights.clear();
}

//=============================
// SCENES
//=============================

void Renderer::AddScene(std::shared_ptr<Scene> scene) {
    scenes.emplace(scene->name, scene);
}

void Renderer::RemoveScene(std::string name) {
    if (scenes.contains(name)) scenes.erase(name);
    else Logger::Warn("[Renderer::RemoveScene] SCENE: " + name + " COULD NOT BE FOUND");
}

//=============================
// ITEMS
//=============================

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

//=============================
// LIGHTS
//=============================

void Renderer::AddLight(std::shared_ptr<WE::Light> light) {
    lights.push_back(light);
}

void Renderer::RemoveLight(std::string p_name) {
    const size_t pre_size = lights.size();

    lights.erase(
        std::remove_if(
            lights.begin(),
            lights.end(),
            [&](const std::shared_ptr<WE::Light> light) {
                return light->name == p_name;
            }
        ),
        lights.end()
    );

    if (lights.size() == pre_size) Logger::Warn("[Renderer::RemoveLight] LIGHT: " + p_name + " COULD NOT BE FOUND");
}

//=============================
// PRIVATE
//=============================

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

            batch.uniforms.model = glGetUniformLocation(shader, "model");
            batch.uniforms.view_projection = glGetUniformLocation(shader, "view_projection");
            batch.uniforms.camera_pos = glGetUniformLocation(shader, "camera_pos");
            batch.uniforms.light_dir = glGetUniformLocation(shader, "light_dir");
            batch.uniforms.light_color = glGetUniformLocation(shader, "light_color");
            batch.uniforms.light_space = glGetUniformLocation(batch.program, "light_space_matrix");
            batch.uniforms.ambient_strength = glGetUniformLocation(shader, "ambient_strength");
            batch.uniforms.specular_strength = glGetUniformLocation(shader, "specular_strength");
            batch.uniforms.shininess = glGetUniformLocation(shader, "shininess");
            batch.uniforms.diffuse = glGetUniformLocation(shader, "u_Diffuse");
            batch.uniforms.shadow_map = glGetUniformLocation(batch.program, "shadow_map");

            items.push_back(std::move(batch));
        }

        items.back().items.push_back(item);
    }

    unbatched_items.clear();
}

void Renderer::_BuildShadows() {
    glGenFramebuffers(1, &shadowFBO);

    glGenTextures(1, &shadow_depth_texture);
    glBindTexture(GL_TEXTURE_2D, shadow_depth_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_resolution, shadow_resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border[] = {1,1,1,1};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_depth_texture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_ComputeLightSpaceMatrix() {
    if (lights.empty()) return;

    auto& light = lights[0];

    glm::vec3 light_dir = glm::normalize(light->direction);
    glm::vec3 light_pos = -light_dir * 30.0f; // pull back along direction

    glm::mat4 light_projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 60.0f);
    glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0,1,0));

    light_space_matrix = light_projection * light_view;
}

void Renderer::_RenderShadowPass() {
    if (lights.empty()) return;

    glViewport(0, 0, shadow_resolution, shadow_resolution);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(shadow_depth_shader);

    GLint lsLoc = glGetUniformLocation(shadow_depth_shader, "lightSpaceMatrix");
    GLint modelLoc = glGetUniformLocation(shadow_depth_shader, "model");

    glUniformMatrix4fv(lsLoc, 1, GL_FALSE, glm::value_ptr(light_space_matrix));

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT); // reduces shadow acne

    for (auto& batch : items) {
        for (auto& item : batch.items) {
            if (!item || !item->active) continue;

            if (item->type != WE::RENDERITEM_TYPE::OBJECT && item->type != WE::RENDERITEM_TYPE::STATIC_OBJECT && item->type != WE::RENDERITEM_TYPE::DYNAMIC_OBJECT) continue;

            auto obj = std::static_pointer_cast<Object>(item->ptr);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obj->GetModelMatrix()));

            obj->Render();
        }
    }

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}