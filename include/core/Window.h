#ifndef WE_CORE_WINDOW_H_
#define WE_CORE_WINDOW_H_

#include <iostream>
#include <memory>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <glm/glm.hpp>
#include <glad/gl.h>

#include "core/WebsterEngine.h"
#include "core/Logger.h"
#include "cfg/launch_options.h"

class Window {
public:
    Window(WE::WINDOW_RESOLUTION resolution, std::string title);
    ~Window();

    std::shared_ptr<double> delta_time = std::make_shared<double>(0.0f);

    void NeedRender();
    bool StartRender();
    void EndRender();

    void UpdateDeltaTime();

    void SetTitle();

    SDL_Window* GetWindow() { return window; };
    SDL_GLContext GetContext() { return context; };
    glm::vec2 GetSize() { return size; };
    WE::WINDOW_RESOLUTION GetResolution() { return resolution; };
    std::string GetTitle() { return title; };
    float GetAspectRatio() { return (size.x / size.y); };

private:
    WE::WINDOW_RESOLUTION resolution;
    std::string title;
    glm::vec2 size;
    
    bool need_render = true;

    SDL_Window* window = NULL;
    SDL_GLContext context = NULL;

    uint64_t last_counter = 0;

    void _ResolutionToSize();
};

#endif // WE_CORE_WINDOW_H_