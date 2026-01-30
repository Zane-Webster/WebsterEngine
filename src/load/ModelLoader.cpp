#include "load/ModelLoader.h"

ModelLoader::ModelLoader() {
    ;
}

std::shared_ptr<Object> ModelLoader::LoadObject(std::string name, std::string path, WE::Material material, WE::COLLIDER_TYPE collider_type, glm::vec3 origin) {
    std::vector<std::unique_ptr<Triangle>> triangles = ModelLoader::_LoadTriangles(path);

    if (triangles.empty()) return nullptr;

    Logger::Info("MODEL: [" + name + "] SUCESSFULLY LOADED");
    return std::make_shared<Object>(name, material, std::move(triangles), collider_type, origin);
}

std::shared_ptr<StaticObject> ModelLoader::LoadStaticObject(std::string name, std::string path, WE::Material material, WE::COLLIDER_TYPE collider_type, glm::vec3 origin) {
    std::vector<std::unique_ptr<Triangle>> triangles = ModelLoader::_LoadTriangles(path);

    if (triangles.empty()) return nullptr;

    Logger::Info("MODEL: [" + name + "] SUCESSFULLY LOADED");
    return std::make_shared<StaticObject>(name, material, std::move(triangles), collider_type, origin);
}

std::shared_ptr<DynamicObject> ModelLoader::LoadDynamicObject(std::string name, std::string path, WE::Material material, WE::COLLIDER_TYPE collider_type, glm::vec3 origin) {
    std::vector<std::unique_ptr<Triangle>> triangles = ModelLoader::_LoadTriangles(path);

    if (triangles.empty()) return nullptr;

    Logger::Info("MODEL: [" + name + "] SUCESSFULLY LOADED");
    return std::make_shared<DynamicObject>(name, material, std::move(triangles), collider_type, origin);
}

std::vector<std::unique_ptr<Triangle>> ModelLoader::_LoadTriangles(std::string path) {
    std::vector<std::unique_ptr<Triangle>> triangles = {};

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality
    );

    if (!scene || !scene->mRootNode) {
        Logger::Error("[ModelLoader] Failed to load: " + path);
        return {};
    }

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace& face = mesh->mFaces[f];

            std::vector<GLfloat> verts = {};

            for (int i = 0; i < 3; i++) {
                unsigned int idx = face.mIndices[i];

                aiVector3D pos  = mesh->mVertices[idx];
                aiVector3D norm = mesh->mNormals[idx];

                aiVector3D uv(0.0f, 0.0f, 0.0f);
                if (mesh->HasTextureCoords(0)) {
                    uv = mesh->mTextureCoords[0][idx];
                }

                // position
                verts.push_back(pos.x);
                verts.push_back(pos.y);
                verts.push_back(pos.z);

                // color
                verts.push_back(default_color.r);
                verts.push_back(default_color.g);
                verts.push_back(default_color.b);

                // uv
                verts.push_back(uv.x);
                verts.push_back(uv.y);
            }

            triangles.push_back(std::make_unique<Triangle>(verts));
        }
    }
    return triangles;
}