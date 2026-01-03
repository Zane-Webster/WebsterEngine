// ===== C++ =====
#include <iostream>
#include <vector>
#include <memory>
#include <string>

// ===== EXTERNAL =====
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <glad/gl.h>

#include <glm/glm.hpp>

// ===== CAMERA =====
#include "cam/Camera.h"

// ===== CONFIG =====
#include "cfg/cfg.h"
#include "cfg/launch_options.h"

// ===== CORE =====
#include "core/WebsterEngine.h"
#include "core/Window.h"
#include "core/Logger.h"

// ===== RENDERER =====
#include "load/ModelLoader.h"

// ===== PRIM =====
#include "prim/Triangle.h"

// ===== RENDERER =====
#include "renderer/Renderer.h"

// ===== SHADER =====
#include "shader/Shader.h"
#include "shader/ShaderHandler.h"

// ===== STATE =====
#include "state/StateHandler.h"

// ===== UTILS =====
#include "utils/Utils.h"

int main(int, char**) {
    WE::KEYSET keyset = WE::KEYSET::WASD;

    Window window(WE_LAUNCH_WINDOW_RESOLUTION, WE::WINDOW_TITLE);
    Renderer renderer;
    StateHandler state_handler;
    ShaderHandler shader_handler;
    ModelLoader model_loader;
    Camera camera(window.GetAspectRatio(), keyset, window.delta_time);

    state_handler.SetState(WE_LAUNCH_STATE);

    std::shared_ptr<Object> ball = model_loader.Load("ball", "assets/objs/ball.obj");

    shader_handler.AddShader("basic", "assets/shaders/basic/frag/triangle.frag", GL_FRAGMENT_SHADER);
    shader_handler.AddShader("basic", "assets/shaders/basic/vert/triangle.vert", GL_VERTEX_SHADER);
    shader_handler.CompileProgram("basic");

    std::shared_ptr<Scene> test_scene = std::make_shared<Scene>("test_scene");
    test_scene->AddItem(std::make_shared<WE::RenderItem>("ball", WE::RENDERITEM_TYPE::OBJECT, shader_handler.GetProgram("basic"), ball));

    while (state_handler.GetState() != WE::STATE::EXIT) {
        window.UpdateDeltaTime();

        // ===============================
        // EDITOR
        // ===============================
        if (state_handler.GetState() == WE::STATE::EDITOR) {
            if (state_handler.Load()) {
                renderer.AddScene(test_scene);
                renderer.Build();
            }

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_EVENT_QUIT: 
                        state_handler.SetState(WE::STATE::EXIT);
                        break;
                    case SDL_EVENT_KEY_DOWN:
                        camera.StartKey(e.key.scancode);
                        break;
                    case SDL_EVENT_KEY_UP: 
                        camera.EndKey(e.key.scancode);
                        break;
                    case SDL_EVENT_MOUSE_MOTION:
                        if (camera.ProcessMouse(static_cast<float>(e.motion.xrel), static_cast<float>(e.motion.yrel))) window.NeedRender();
                        break;
                };
            }

            if (camera.ProcessKey()) window.NeedRender();

            if (window.StartRender()) {
                renderer.RenderAll(camera.GetViewProjectionMatrix());

                window.EndRender();
            }
        }
    }

    // ===============================
    // EXIT
    // ===============================

    renderer.Clear();
    test_scene->Clear();

    shader_handler.Destroy();
    ball->Destroy();

    SDL_Quit();

    return 0;
}
    