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
#include "utils/GLMStream.h"
#include "utils/CollisionUtils.h"

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

    std::shared_ptr<DynamicObject> ball = model_loader.LoadDynamicObject("ball", "assets/obj/ball/ball.obj", basic_material, WE::COLLIDER_TYPE::SPHERE, glm::vec3(0.0f, 2.0f, 0.0f));

    std::shared_ptr<DynamicObject> box1 = model_loader.LoadDynamicObject("box1", "assets/obj/box/box.obj", basic_material, WE::COLLIDER_TYPE::AABB, glm::vec3(3.0f, 0.0f, -0.5f));
    std::shared_ptr<DynamicObject> box2 = model_loader.LoadDynamicObject("box2", "assets/obj/box/box.obj", basic_material, WE::COLLIDER_TYPE::AABB, glm::vec3(3.0f, 0.0f, 0.5f));
    std::shared_ptr<DynamicObject> box3 = model_loader.LoadDynamicObject("box3", "assets/obj/box/box.obj", basic_material, WE::COLLIDER_TYPE::AABB, glm::vec3(3.0f, 10.0f, -0.5f));
    std::shared_ptr<DynamicObject> box4 = model_loader.LoadDynamicObject("box4", "assets/obj/box/box.obj", basic_material, WE::COLLIDER_TYPE::AABB, glm::vec3(3.0f, 10.0f, 0.5f));
    std::shared_ptr<DynamicObject> box5 = model_loader.LoadDynamicObject("box5", "assets/obj/box/box.obj", basic_material, WE::COLLIDER_TYPE::AABB, glm::vec3(3.0f, 20.0f, -0.5f));
    std::shared_ptr<DynamicObject> box6 = model_loader.LoadDynamicObject("box6", "assets/obj/box/box.obj", basic_material, WE::COLLIDER_TYPE::AABB, glm::vec3(3.0f, 20.0f, 0.5f));

    std::shared_ptr<StaticObject> floor = model_loader.LoadStaticObject("floor", "assets/obj/floor/floor.obj", matte, WE::COLLIDER_TYPE::AABB, glm::vec3(0.0f, -4.0f, 0.0f));
    std::shared_ptr<StaticObject> wall = model_loader.LoadStaticObject("wall", "assets/obj/wall/wall.obj", matte, WE::COLLIDER_TYPE::AABB, glm::vec3(-7.0f, 0.0f, 0.0f));

    box1->SetDynamicProperties(1.0f, 6.0f, 0.3f);
    box2->SetDynamicProperties(1.0f, 6.0f, 0.3f);
    box3->SetDynamicProperties(1.0f, 6.0f, 0.3f);
    box4->SetDynamicProperties(1.0f, 6.0f, 0.3f);
    box5->SetDynamicProperties(1.0f, 6.0f, 0.3f);
    box6->SetDynamicProperties(1.0f, 6.0f, 0.3f);

    ball->SetDynamicProperties(0.5f, 10.0f, 1.0f);

    wall->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));

    box1->SetColor(glm::vec3(0.0f, 0.0f, 1.0f));
    box2->SetColor(glm::vec3(0.0f, 1.0f, 1.0f));
    box3->SetColor(glm::vec3(0.0f, 1.0f, 1.0f));
    box4->SetColor(glm::vec3(0.0f, 0.0f, 1.0f));
    box5->SetColor(glm::vec3(0.0f, 0.0f, 1.0f));
    box6->SetColor(glm::vec3(0.0f, 1.0f, 1.0f));

    ball->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));

    shader_handler.AddShader("basic", "assets/shader/basic/frag/basic.frag", GL_FRAGMENT_SHADER);
    shader_handler.AddShader("basic", "assets/shader/basic/vert/basic.vert", GL_VERTEX_SHADER);
    shader_handler.CompileProgram("basic");

    std::shared_ptr<Scene> test_scene = std::make_shared<Scene>("test_scene");
    test_scene->AddItem(std::make_shared<WE::RenderItem>("ball", WE::RENDERITEM_TYPE::DYNAMIC_OBJECT, shader_handler.GetProgram("basic"), ball));
    /*
    test_scene->AddItem(std::make_shared<WE::RenderItem>("box1", WE::RENDERITEM_TYPE::DYNAMIC_OBJECT, shader_handler.GetProgram("basic"), box1));
    test_scene->AddItem(std::make_shared<WE::RenderItem>("box2", WE::RENDERITEM_TYPE::DYNAMIC_OBJECT, shader_handler.GetProgram("basic"), box2));
    test_scene->AddItem(std::make_shared<WE::RenderItem>("box3", WE::RENDERITEM_TYPE::DYNAMIC_OBJECT, shader_handler.GetProgram("basic"), box3));
    test_scene->AddItem(std::make_shared<WE::RenderItem>("box4", WE::RENDERITEM_TYPE::DYNAMIC_OBJECT, shader_handler.GetProgram("basic"), box4));
    test_scene->AddItem(std::make_shared<WE::RenderItem>("box5", WE::RENDERITEM_TYPE::DYNAMIC_OBJECT, shader_handler.GetProgram("basic"), box5));
    test_scene->AddItem(std::make_shared<WE::RenderItem>("box6", WE::RENDERITEM_TYPE::DYNAMIC_OBJECT, shader_handler.GetProgram("basic"), box6));
    */
    test_scene->AddItem(std::make_shared<WE::RenderItem>("floor", WE::RENDERITEM_TYPE::STATIC_OBJECT, shader_handler.GetProgram("basic"), floor));
    test_scene->AddItem(std::make_shared<WE::RenderItem>("wall", WE::RENDERITEM_TYPE::STATIC_OBJECT, shader_handler.GetProgram("basic"), wall));
    test_scene->AddLight(sun_light);

    SDL_Delay(1500);

    while (state_handler.GetState() != WE::STATE::EXIT) {
        window.UpdateDeltaTime();

        // ===============================
        // EDITOR
        // ===============================
        if (state_handler.GetState() == WE::STATE::EDITOR) {
            if (state_handler.Load()) {
                renderer.Clear();
                renderer.AddScene(test_scene);
                test_scene->Reload();
                renderer.Build();

                window.NeedRender();
                window.UpdateDeltaTime();
            }

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_EVENT_QUIT: 
                        state_handler.SetState(WE::STATE::EXIT);
                        break;
                    case SDL_EVENT_KEY_DOWN:
                        camera.StartKey(e.key.scancode);

                        if (e.key.scancode == SDL_SCANCODE_1) {
                            ball->ResetToOrigin();
                            ball->ResetPhysics();
                        }
                        if (e.key.scancode == SDL_SCANCODE_2) {
                            state_handler.Reload();
                        }
                        if (e.key.scancode == SDL_SCANCODE_LEFT) ball->ApplyImpulse(glm::vec3(-50.0f, 0.0f, 0.0f));
                        if (e.key.scancode == SDL_SCANCODE_RIGHT) ball->ApplyImpulse(glm::vec3(50.0f, 0.0f, 0.0f));
                        if (e.key.scancode == SDL_SCANCODE_UP) ball->ApplyImpulse(glm::vec3(0.0f, 50.0f, 0.0f));
                        if (e.key.scancode == SDL_SCANCODE_DOWN) ball->ApplyImpulse(glm::vec3(0.0f, -50.0f, 0.0f));

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
                            ball->ApplyImpulse(-hit.normal*25.0f);
                            window.NeedRender();
                        }
                        break;
                }
            }

            if (camera.ProcessKey()) window.NeedRender();

            if (test_scene->ProcessPhysics(*window.delta_time)) {
                test_scene->ProcessCollisions(*window.delta_time);
                test_scene->ApplyPhysics();
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
    