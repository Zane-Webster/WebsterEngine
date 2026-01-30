#ifndef WE_SKY_SKYBOX_H_
#define WE_SKY_SKYBOX_H_

#include <iostream>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "tex/Texture.h"

class Skybox {
public:
    Skybox(WE::SKYBOX_TEXTURES textures);

    void Render();
    void Destroy();
    
private:
    WE::SKYBOX_TEXTURES textures = WE_EMPTY_VECTOR;
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint cubemap_texture = 0;

    void _SetupMesh();
    void _BuildCubemap();
};

#endif // WE_SKY_SKYBOX_H_