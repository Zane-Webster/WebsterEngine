#ifndef WE_TEX_TEXTUREHANDLER_H_
#define WE_TEX_TEXTUREHANDLER_H_

#include <iostream>
#include <unordered_map>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "tex/Texture.h"

class TextureHandler {
public:
    TextureHandler();

    void Destroy();

    std::shared_ptr<Texture> LoadTexture(std::string name, std::string path);
    std::shared_ptr<Texture> GetTexture(std::string name);
    
private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures = {};
    
};

#endif // WE_TEX_TEXTUREHANDLER_H_