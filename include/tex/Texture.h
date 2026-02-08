#ifndef WE_TEX_TEXTURE_H_
#define WE_TEX_TEXTURE_H_

#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

class Texture {
public:
    Texture(std::string name, std::string path);

    void Destroy();
    void Bind(uint32_t slot);

    void ScreenRender();

    std::string name = WE_EMPTY_STRING;
    int width = 0;
    int height = 0;
    int channels = 0;
    GLenum format = 0;
    GLenum internal_format = 0;
    unsigned char* pixels = nullptr;

    GLuint texture = 0;

private:
    SDL_Surface* surface = nullptr;
    std::string path = WE_EMPTY_STRING;

    void _LoadImage();
};

#endif // WE_TEX_TEXTURE_H_