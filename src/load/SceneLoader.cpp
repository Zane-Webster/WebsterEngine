#include "load/SceneLoader.h"

SceneLoader::SceneLoader(std::shared_ptr<ModelLoader> p_model_loader) : model_loader(p_model_loader) {
    ;
}

std::vector<std::shared_ptr<WE::RenderItem>> SceneLoader::LoadScene(std::string path, std::unordered_map<std::string, WE::Material> materials, std::unordered_map<std::string, GLuint> shader_programs) {
    return SceneLoader::_ReadFile(path, materials, shader_programs);
}

std::vector<std::shared_ptr<WE::RenderItem>> SceneLoader::_ReadFile(std::string path, std::unordered_map<std::string, WE::Material> materials, std::unordered_map<std::string, GLuint> shader_programs) {
    std::vector<std::shared_ptr<WE::RenderItem>> items = {};

    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::Error("FAILED TO OPEN SCENE FILE: " + path);
        return {};
    }

    std::string line = "";
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        items.push_back(SceneLoader::_ReadItem(line, materials, shader_programs));
    }

    return items;
}

std::shared_ptr<WE::RenderItem> SceneLoader::_ReadItem(std::string line, std::unordered_map<std::string, WE::Material> materials, std::unordered_map<std::string, GLuint> shader_programs) {
    std::stringstream ss(line);

    std::string type_str = "";
    ss >> type_str;

    std::string name = "";
    std::string model_path = "";
    std::string material_name = "";
    std::string shader_name = "";
    std::string collider_type_str = "";
    glm::vec3 origin = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(0.0f);

    ss >> std::ws;
    ss.get();
    std::getline(ss, name, '"');

    ss >> std::ws;
    ss.get();
    std::getline(ss, model_path, '"');

    ss >> std::ws;
    ss.get();
    std::getline(ss, material_name, '"');

    ss >> std::ws;
    ss.get();
    std::getline(ss, shader_name, '"');

    ss >> collider_type_str;

    ss >> origin.x >> origin.y >> origin.z;
    
    bool has_color = false;

    // check if colored
    std::streampos color_pos = ss.tellg();
    if (ss >> color.r >> color.g >> color.b) {
        has_color = true;
    } else {
        ss.clear();
        ss.seekg(color_pos);
    }

    WE::Material material = materials.at(material_name);
    GLuint shader_program = shader_programs.at(shader_name);

    WE::COLLIDER_TYPE collider = Utils::ColliderTypeFromString(collider_type_str);

    // ============================
    // Static object
    // ============================
    if (type_str == "staticobject") {
        auto obj = model_loader->LoadStaticObject(name, model_path, material, collider, origin );

        if (has_color) obj->SetColor(color);

        return std::make_shared<WE::RenderItem>(name, WE::RENDERITEM_TYPE::STATIC_OBJECT, shader_program, obj);
    }

    // ============================
    // Dynamic object
    // ============================
    if (type_str == "dynamicobject") {
        float mass = 2.0f;
        float max_speed = 4.0f;
        float restitution = 0.7f;
        bool use_gravity = true;

        ss >> mass >> max_speed >> restitution >> use_gravity;

        auto obj = model_loader->LoadDynamicObject(name, model_path, material, collider, origin);

        obj->SetDynamicProperties(mass, max_speed, restitution, use_gravity);

        if (has_color) obj->SetColor(color);

        return std::make_shared<WE::RenderItem>(name, WE::RENDERITEM_TYPE::DYNAMIC_OBJECT, shader_program, obj);
    }

    Logger::Error("UNKNOWN RENDERITEM TYPE: " + type_str);
    return nullptr;
}