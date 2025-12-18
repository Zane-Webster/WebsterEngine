#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include "cfg.h"
#include "core/WebsterEngine.h"
#include "core/Window.h"
#include "core/Logger.h"

int main(int, char**) {
    Window window(WE_WINDOW_RESOLUTION::HD, "Webster Engine | 0.1.0");

    bool running = true;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        if (window.StartRender()) {

            window.EndRender();
        }
    }

    SDL_Quit();

    return 0;
}
    