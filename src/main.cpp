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

// ===== LOAD =====
#include "load/ModelLoader.h"

// ===== OBJ =====
#include "obj/Object.h"
#include "obj/StaticObject.h"
#include "obj/DynamicObject.h"

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

    std::shared_ptr<WE::Light> sun_light = std::make_shared<WE::Light>("sun", glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f)), glm::vec3(1.0f, 0.95f, 0.9f));
    WE::Material basic_material = {0.15f, 0.5f, 32.0f};
    WE::Material matte = {0.15f, 0.0f, 0.0f};

    std::shared_ptr<DynamicObject> ball = model_loader.LoadDynamicObject("ball", "assets/obj/ball/ball.obj", basic_material, WE::COLLIDER_TYPE::SPHERE);
    std::shared_ptr<StaticObject> floor = model_loader.LoadStaticObject("floor", "assets/obj/floor/floor.obj", matte);

    shader_handler.AddShader("basic", "assets/shader/basic/frag/basic.frag", GL_FRAGMENT_SHADER);
    shader_handler.AddShader("basic", "assets/shader/basic/vert/basic.vert", GL_VERTEX_SHADER);
    shader_handler.CompileProgram("basic");

    std::shared_ptr<Scene> test_scene = std::make_shared<Scene>("test_scene");
    test_scene->AddItem(std::make_shared<WE::RenderItem>("ball", WE::RENDERITEM_TYPE::DYNAMIC_OBJECT, shader_handler.GetProgram("basic"), ball));
    test_scene->AddItem(std::make_shared<WE::RenderItem>("floor", WE::RENDERITEM_TYPE::STATIC_OBJECT, shader_handler.GetProgram("basic"), floor));
    test_scene->AddLight(sun_light);

    while (state_handler.GetState() != WE::STATE::EXIT) {
        window.UpdateDeltaTime();

        // ===============================
        // EDITOR
        // ===============================
        if (state_handler.GetState() == WE::STATE::EDITOR) {
            if (state_handler.Load()) {
                renderer.Clear();
                renderer.AddScene(test_scene);
                renderer.Build();

                window.NeedRender();
            }

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_EVENT_QUIT: 
                        state_handler.SetState(WE::STATE::EXIT);
                        break;
                    case SDL_EVENT_KEY_DOWN:
                        camera.StartKey(e.key.scancode);

                        if (e.key.scancode == SDL_SCANCODE_1) test_scene->GetObject("ball")->ResetToOrigin();
                        if (e.key.scancode == SDL_SCANCODE_2) {
                            test_scene->RemoveItem("ball");
                            state_handler.Reload();
                        }

                        if (e.key.scancode == SDL_SCANCODE_DELETE) state_handler.SetState(WE::STATE::EXIT);

                        break;
                    case SDL_EVENT_KEY_UP: 
                        camera.EndKey(e.key.scancode);
                        break;
                    case SDL_EVENT_MOUSE_MOTION:
                        if (camera.ProcessMouse(static_cast<float>(e.motion.xrel), static_cast<float>(e.motion.yrel))) window.NeedRender();
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        WE::RayHit hit;
                        if (test_scene->Raycast(camera.GetForwardRay(), hit)) {
                            test_scene->GetObject("ball")->Translate(glm::vec3(0.0f, 0.1f, 0.0f));
                            window.NeedRender();
                        }
                        break;
                };
            }

            if (camera.ProcessKey()) window.NeedRender();

            if (ball->moving) {
                ball->ProcessPhysics();
                window.NeedRender();
            }

            if (window.StartRender()) {
                renderer.RenderAll(camera.GetViewProjectionMatrix(), camera.GetPosition());

                window.EndRender();
            }
        }
    }

    // ===============================
    // EXIT
    // ===============================

    renderer.Clear();

    test_scene->Destroy();

    shader_handler.Destroy();
    ball->Destroy();

    SDL_Quit();

    return 0;
}
    