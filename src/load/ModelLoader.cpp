#include "load/ModelLoader.h"

ModelLoader::ModelLoader() {
    ;
}

std::unique_ptr<Object> ModelLoader::Load(std::string name, std::string path) {
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
        return nullptr;
    }

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace& face = mesh->mFaces[f];

            std::vector<GLfloat> verts = {};

            for (int i = 0; i < 3; i++) {
                unsigned int idx = face.mIndices[i];

                aiVector3D pos = mesh->mVertices[idx];
                aiVector3D norm = mesh->mNormals[idx];

                // position
                verts.push_back(pos.x);
                verts.push_back(pos.y);
                verts.push_back(pos.z);

                // color
                verts.push_back(default_color.r);
                verts.push_back(default_color.g);
                verts.push_back(default_color.b);
            }

            triangles.push_back(std::make_unique<Triangle>(verts));
        }
    }

    Logger::Info("MODEL: [" + name + "] SUCESSFULLY LOADED");
    return std::make_unique<Object>(name, std::move(triangles));
}