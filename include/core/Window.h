#ifndef SRC_WINDOW_H_
#define SRC_WINDOW_H_

#include <iostream>
#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <glad/gl.h>

#include "core/WebsterEngine.h"
#include "core/Logger.h"
#include "cfg/launch_options.h"

class Window {
public:
    Window(WE_WINDOW_RESOLUTION resolution, std::string title);
    ~Window();

    void NeedRender();
    bool StartRender();
    void EndRender();

    void SetTitle();

    SDL_Window* GetWindow() { return window; };
    SDL_GLContext GetContext() { return context; };
    glm::vec2 GetSize() { return size; };
    WE_WINDOW_RESOLUTION GetResolution() { return resolution; };
    std::string GetTitle() { return title; };

private:
    WE_WINDOW_RESOLUTION resolution;
    std::string title;
    glm::vec2 size;
    
    bool need_render = true;

    SDL_Window* window = NULL;
    SDL_GLContext context = NULL;

    void _ResolutionToSize();
};

#endif // SRC_WINDOW_H_