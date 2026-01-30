#include "tex/Texture.h"

Texture::Texture(std::string p_name, std::string p_path) : name(p_name), path(p_path) {
    Texture::_LoadImage();
}

void Texture::Destroy() {
    glDeleteTextures(1, &texture);
    texture = 0;
    delete[] pixels;
    pixels = nullptr;
}

void Texture::Bind(uint32_t slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::_LoadImage() {
    surface = IMG_Load(path.c_str());
    if (!surface) {
        Logger::Error("FAILED TO LOAD TEXTURE: " + name);
        return;
    }

    const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(surface->format);

    format = (details->bytes_per_pixel == 4) ? GL_RGBA : GL_RGB;
    internal_format = (format == GL_RGBA) ? GL_RGBA8 : GL_RGB8;
    width = surface->w;
    height = surface->h;
    channels = (details->bytes_per_pixel == 4) ? 4 : 3;

    size_t size = width * height * channels;
    pixels = new unsigned char[size];
    memcpy(pixels, surface->pixels, size);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_DestroySurface(surface);
    surface = nullptr;
}