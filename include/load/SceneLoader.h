#ifndef WE_LOAD_SCENELOADER_H_
#define WE_LOAD_SCENELOADER_H_

#include <iostream>
#include <unordered_map>
#include <fstream>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "utils/utils.h"

#include "load/ModelLoader.h"
#include "tex/TextureHandler.h"

#include "obj/Object.h"
#include "obj/DynamicObject.h"
#include "obj/StaticObject.h"

class SceneLoader {
public:
    SceneLoader(std::shared_ptr<ModelLoader> model_loader);

    std::vector<std::shared_ptr<WE::RenderItem>> LoadScene(std::string path, std::unordered_map<std::string, WE::Material> materials, std::unordered_map<std::string, GLuint> shader_programs);
    
private:
    std::shared_ptr<ModelLoader> model_loader = nullptr;

    std::vector<std::shared_ptr<WE::RenderItem>> _ReadFile(std::string path, std::unordered_map<std::string, WE::Material> materials, std::unordered_map<std::string, GLuint> shader_programs);
    std::shared_ptr<WE::RenderItem> _ReadItem(std::string line, std::unordered_map<std::string, WE::Material> materials, std::unordered_map<std::string, GLuint> shader_programs);
    
};

#endif // WE_LOAD_SCENELOADER_H_