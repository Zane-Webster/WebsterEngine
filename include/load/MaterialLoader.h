#ifndef WE_LOAD_MATERIALLOADER_H_
#define WE_LOAD_MATERIALLOADER_H_

#include <iostream>
#include <unordered_map>
#include <fstream>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "tex/TextureHandler.h"

class MaterialLoader {
public:
    MaterialLoader(std::shared_ptr<TextureHandler> texture_handler);

    WE::Material LoadMaterial(std::string path);

    std::unordered_map<std::string, WE::Material> GetAllMaterials();
    
private:
    std::shared_ptr<TextureHandler> texture_handler = nullptr;

    std::unordered_map<std::string, WE::Material> materials = {};

    WE::Material _ReadFile(std::string path);
    

};

#endif // WE_LOAD_MATERIALLOADER_H_