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
#include "obj/StaticObject.h"
#include "obj/DynamicObject.h"

class ModelLoader {
public:
    ModelLoader();

    std::shared_ptr<Object> LoadObject(std::string name, std::string path, WE::Material material);
    std::shared_ptr<StaticObject> LoadStaticObject(std::string name, std::string path, WE::Material material);
    std::shared_ptr<DynamicObject> LoadDynamicObject(std::string name, std::string path, WE::Material material);

private:
    glm::vec3 default_color = glm::vec3(0.50f);
    std::vector<std::unique_ptr<Triangle>> _LoadTriangles(std::string path);

};

#endif // WE_LOAD_MODELLOADER_H_