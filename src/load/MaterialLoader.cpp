#include "load/MaterialLoader.h"

MaterialLoader::MaterialLoader(std::shared_ptr<TextureHandler> p_texture_handler) : texture_handler(p_texture_handler) {
    ;
}

WE::Material MaterialLoader::LoadMaterial(std::string path) {
    return MaterialLoader::_ReadFile(path);
}

std::unordered_map<std::string, WE::Material> MaterialLoader::GetAllMaterials() {
    return materials;
}

WE::Material MaterialLoader::_ReadFile(std::string path) {
    WE::Material material = {};

    std::string material_name = "";

    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::Error("FAILED TO OPEN MATERIAL FILE: " + path);
        return {};
    }

    std::string line = "";
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);

        ss >> std::ws;
        ss.get();
        std::getline(ss, material_name, '"');

        ss >> material.ambient_strength;
        ss >> material.specular_strength;
        ss >> material.shininess;

        ss >> std::ws;
        if (ss.peek() == '"') {
            std::string texture_name = "";
            std::string texture_path = "";

            ss.get();
            std::getline(ss, texture_name, '"');
            
            ss >> std::ws;
            ss.get();
            std::getline(ss, texture_path, '"');

            material.diffuse = texture_handler->LoadTexture(texture_name, texture_path);
        }

        materials[material_name] = material;
        return material;
    }

    Logger::Error("COULD NOT LOAD MATERIAL AT: " + path);
    return {};
}