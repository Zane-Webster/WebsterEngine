#include "core/Window.h"

Window::Window(WE::WINDOW_RESOLUTION p_resolution, std::string p_title) : resolution(p_resolution), title(p_title) {
    if (!SDL_Init(SDL_INIT_VIDEO)) Logger::Error("SDL VIDEO FAILED TO INIT");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // convert resolution enum to size vec2
    Window::_ResolutionToSize();

    window = SDL_CreateWindow(title.c_str(), static_cast<int>(size.x), static_cast<int>(size.y), SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) Logger::Error("FAILED TO CREATE WINDOW");

    context = SDL_GL_CreateContext(window);
    if (!context) Logger::Error("FAILED TO CREATE OPENGL CONTEXT");

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) Logger::Error("FAILED TO INIT GLAD");

    // gl settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    SDL_GL_MakeCurrent(window, context);

    SDL_SetWindowRelativeMouseMode(window, true);
    
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

void Window::UpdateDeltaTime() {
    const uint64_t freq = SDL_GetPerformanceFrequency();

    uint64_t now = SDL_GetPerformanceCounter();

    if (last_counter == 0) {
        last_counter = now;
        *delta_time = 0.0;
        return;
    }

    // time elapsed since last frame
    double frame_time = static_cast<double>(now - last_counter) / static_cast<double>(freq);

    // ===============================
    // FPS LIMITING (no vsync)
    // ===============================
    if (!WE_LAUNCH_VSYNC) {
        const double target_frame_time = 1.0 / static_cast<double>(WE_LAUNCH_FPS_CAP);

        if (frame_time < target_frame_time) {
            double remaining = target_frame_time - frame_time;

            // sleep most of the remaining time (milliseconds)
            if (remaining > 0.001) {
                SDL_Delay(static_cast<Uint32>(remaining * 1000.0));
            }

            // busy-wait the final microseconds for precision
            do {
                now = SDL_GetPerformanceCounter();
                frame_time = static_cast<double>(now - last_counter) / static_cast<double>(freq);
            } while (frame_time < target_frame_time);
        }
    }

    *delta_time = frame_time;
    last_counter = now;
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