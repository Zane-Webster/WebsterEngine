#ifndef WE_LOAD_MODELLOADER_H_
#define WE_LOAD_MODELLOADER_H_

#include <iostream>
#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "prim/Triangle.h"

#include "obj/Object.h"

class ModelLoader {
public:
    ModelLoader();

    std::shared_ptr<Object> Load(std::string name, std::string path, WE::Material material);

private:
    glm::vec3 default_color = glm::vec3(0.50f);

};

#endif // WE_LOAD_MODELLOADER_H_