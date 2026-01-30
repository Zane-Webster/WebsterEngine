#ifndef WE_RENDERER_RENDERER_H_
#define WE_RENDERER_RENDERER_H_

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

#include <glm/gtc/type_ptr.hpp>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "obj/Object.h"
#include "obj/StaticObject.h"
#include "obj/DynamicObject.h"
#include "sky/Skybox.h"

#include "tex/Texture.h"

#include "scene/Scene.h"

class Renderer {
public:
    Renderer();

    // ======== BASIC ========
    void Build();
    void RenderAll(glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::mat4 view_projection_matrix, glm::vec3 camera_pos);
    void Clear();

    // ======== SCENES ========
    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(std::string name);

    // ======== ITEMS ========
    void AddItem(std::shared_ptr<WE::RenderItem> item);
    void AddItems(std::vector<std::shared_ptr<WE::RenderItem>> items);

    void RemoveItem(std::string name);
    void RemoveItems(std::vector<std::string> names);

    // ======== LIGHTS ========
    void AddLight(std::shared_ptr<WE::Light> light);
    void RemoveLight(std::string name);

    std::shared_ptr<Texture> basic_texture = nullptr;
    
private:
    std::vector<std::shared_ptr<WE::RenderItem>> unbatched_items = WE_EMPTY_VECTOR;
    std::vector<WE::RenderBatch> items = WE_EMPTY_VECTOR;
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes = WE_EMPTY_MAP;
    std::unordered_map<GLuint, WE::ShaderUniforms> uniforms = WE_EMPTY_MAP;\
    std::vector<std::shared_ptr<WE::Light>> lights = WE_EMPTY_VECTOR;


    void _MakeBatches();
};

#endif // WE_RENDERER_RENDERER_H_