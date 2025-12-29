// ===== C++ =====
#include <iostream>

// ===== EXTERNAL =====
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <glad/gl.h>

#include <glm/glm.hpp>

// ===== CONFIG =====
#include "cfg/cfg.h"
#include "cfg/launch_options.h"

// ===== CORE =====
#include "core/WebsterEngine.h"
#include "core/Window.h"
#include "core/Logger.h"

// ===== PRIM =====
#include "prim/Triangle.h"

// ===== RENDERER =====

// ===== SHADER =====
#include "shader/Shader.h"
#include "shader/ShaderHandler.h"

// ===== STATE =====
#include "state/StateHandler.h"

// ===== UTILS =====
#include "utils/Utils.h"

int main(int, char**) {
    Window window(WE_LAUNCH_WINDOW_RESOLUTION, "Webster Engine | 0.1.0");
    StateHandler state_handler;
    ShaderHandler shader_handler;

    state_handler.SetState(WE_LAUNCH_STATE);

    Triangle tri({
        -0.5f, -0.5f, 0.0f, 1.f, 0.f, 0.f,
        0.5f, -0.5f, 0.0f, 0.f, 1.f, 0.f,
        0.0f,  0.5f, 0.0f, 0.f, 0.f, 1.f
    });

    shader_handler.AddShader("basic", "assets/shaders/basic/frag/triangle.frag", GL_FRAGMENT_SHADER);
    shader_handler.AddShader("basic", "assets/shaders/basic/vert/triangle.vert", GL_VERTEX_SHADER);
    shader_handler.CompileProgram("basic");

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
                glUseProgram(shader_handler.GetProgram("basic"));
                tri.Render();
                window.EndRender();
            }
        }
    }

    shader_handler.Destroy();
    tri.Destroy();

    SDL_Quit();

    return 0;
}
    