#include "core/Window.h"

Window::Window(WE::WINDOW_RESOLUTION p_resolution, std::string p_title) : resolution(p_resolution), title(p_title) {
    if (!SDL_Init(SDL_INIT_VIDEO)) Logger::Error("SDL VIDEO FAILED TO INIT");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // convert resolution enum to size vec2
    Window::_ResolutionToSize();

    window = SDL_CreateWindow(title.c_str(), size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) Logger::Error("FAILED TO CREATE WINDOW");

    context = SDL_GL_CreateContext(window);
    if (!context) Logger::Error("FAILED TO CREATE OPENGL CONTEXT");

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) Logger::Error("FAILED TO INIT GLAD");

    SDL_GL_MakeCurrent(window, context);

    if (WE_LAUNCH_VSYNC) SDL_GL_SetSwapInterval(1);
}

Window::~Window() {
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);;
}

void Window::NeedRender() {
    need_render = true;
}

bool Window::StartRender() {
    if (need_render) { 
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return true;
    }
    return false;
}

void Window::EndRender() {
    SDL_GL_SwapWindow(window);
    need_render = false;
}

void Window::SetTitle() {
    ;
}

void Window::_ResolutionToSize() {
    switch (resolution) {
        case WE::WINDOW_RESOLUTION::HD:
            size = glm::vec2(1280, 720);
            break;
        case WE::WINDOW_RESOLUTION::FHD:
            size = glm::vec2(1920, 1080);
            break;
        case WE::WINDOW_RESOLUTION::QHD:
            size = glm::vec2(2560, 1440);
            break;
    }
}