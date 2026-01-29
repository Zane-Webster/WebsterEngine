#include "tex/TextureHandler.h"

TextureHandler::TextureHandler() {
    ;
}

void TextureHandler::Destroy() {
    for (auto& [name, tex] : textures) {
        tex->Destroy();
    }
    textures.clear();
}

void TextureHandler::LoadTexture(std::string name, std::string path) {
    if (textures.count(name) > 0) {
        Logger::Error("TEXTURE NAMED: " + name + " ALREADY EXISTS");
        return;
    }
    textures[name] = std::make_shared<Texture>(name, path);
}

std::shared_ptr<Texture> TextureHandler::GetTexture(std::string name) {
     if (textures.count(name) == 0) {
        Logger::Error("TEXTURE NAMED: " + name + " NOT FOUND.");
        return nullptr;
    }
    return textures[name];
}