#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include "cfg.h"
#include "core/WebsterEngine.h"
#include "core/Window.h"
#include "core/Logger.h"
#include "state/StateHandler.h"

int main(int, char**) {
    Window window(WE_WINDOW_RESOLUTION::HD, "Webster Engine | 0.1.0");
    StateHandler state_handler;

    state_handler.SetState(WE_STATE::EDITOR);

    while (state_handler.GetState() != WE_STATE::EXIT) {
        // ===============================
        // EDITOR
        // ===============================
        if (state_handler.GetState() == WE_STATE::EDITOR) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) state_handler.SetState(WE_STATE::EXIT);
            }

            if (window.StartRender()) {

                window.EndRender();
            }
        }
    }

    SDL_Quit();

    return 0;
}
    