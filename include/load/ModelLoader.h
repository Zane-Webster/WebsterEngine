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

class ModelLoader {
public:
    ModelLoader();

    std::vector<std::unique_ptr<Triangle>> Load(std::string path);

private:
    glm::vec3 default_color = glm::vec3(0.50f);

};

#endif // WE_LOAD_MODELLOADER_H_