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
#include "load/MaterialLoader.h"
#include "load/ModelLoader.h"
#include "load/SceneLoader.h"

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

// ===== SKYBOX =====
#include "sky/Skybox.h"

// ===== STATE =====
#include "state/StateHandler.h"

// ===== TEXTURES =====
#include "tex/Texture.h"
#include "tex/TextureHandler.h"

// ===== UTILS =====
#include "utils/Utils.h"
#include "utils/GLMStream.h"
#include "utils/CollisionUtils.h"

int main(int, char**) {
    WE::KEYSET keyset = WE::KEYSET::WASD;

    Window window(WE_LAUNCH_WINDOW_RESOLUTION, WE::WINDOW_TITLE);
    Renderer renderer((Uint16)window.GetSize().x, (Uint16)window.GetSize().y);
    StateHandler state_handler;
    ShaderHandler shader_handler;
    std::shared_ptr<TextureHandler> texture_handler = std::make_shared<TextureHandler>();
    MaterialLoader material_loader(texture_handler);
    std::shared_ptr<ModelLoader> model_loader = std::make_shared<ModelLoader>();
    SceneLoader scene_loader(model_loader);
    Camera camera(window.GetAspectRatio(), keyset, window.delta_time);

    // ===============================
    // SET STATES AND INITS
    // ===============================

    state_handler.SetState(WE_LAUNCH_STATE);

    texture_handler->LoadTexture("basic", "assets/tex/basic.png");
    renderer.basic_texture = texture_handler->GetTexture("basic");

    // ===============================
    // COMPILE SHADERS
    // ===============================
    
    shader_handler.AddShader("basic", "assets/shader/basic/frag/basic.frag", GL_FRAGMENT_SHADER);
    shader_handler.AddShader("basic", "assets/shader/basic/vert/basic.vert", GL_VERTEX_SHADER);
    shader_handler.CompileProgram("basic");

    shader_handler.AddShader("sky", "assets/shader/sky/frag/sky.frag", GL_FRAGMENT_SHADER);
    shader_handler.AddShader("sky", "assets/shader/sky/vert/sky.vert", GL_VERTEX_SHADER);
    shader_handler.CompileProgram("sky");

    shader_handler.AddShader("shadow", "assets/shader/shadow/frag/shadow.frag", GL_FRAGMENT_SHADER);
    shader_handler.AddShader("shadow", "assets/shader/shadow/vert/shadow.vert", GL_VERTEX_SHADER);
    shader_handler.CompileProgram("shadow");

    // ===============================
    // LOAD MATERIALS
    // ===============================

    material_loader.LoadMaterial("assets/material/basic.wemat");
    material_loader.LoadMaterial("assets/material/matte_brick.wemat");
    material_loader.LoadMaterial("assets/material/matte_pavement.wemat");

    // ===============================
    // LOAD SCENES
    // ===============================

    std::shared_ptr<Scene> demo_scene = std::make_shared<Scene>("demo");
    demo_scene->AddItems(scene_loader.LoadScene("assets/scene/demo.wescn", material_loader.GetAllMaterials(), shader_handler.GetAllPrograms()));

    std::shared_ptr<DynamicObject> ball = std::dynamic_pointer_cast<DynamicObject>(demo_scene->GetObject("ball"));

    // ===============================
    // LOAD SKYBOXES AND LIGHTS
    // ===============================

    std::shared_ptr<WE::Light> sun_light = std::make_shared<WE::Light>("sun", glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f)), glm::vec3(1.0f, 0.95f, 0.9f));
    demo_scene->AddLight(sun_light);

    texture_handler->LoadTexture("sky_side1", "assets/tex/sky/side1.png");
    texture_handler->LoadTexture("sky_side2", "assets/tex/sky/side2.png");

    WE::SKYBOX_TEXTURES skybox_textures = { texture_handler->GetTexture("sky_side1"), 
                                            texture_handler->GetTexture("sky_side1"), 
                                            texture_handler->LoadTexture("sky_top", "assets/tex/sky/top.png"), 
                                            texture_handler->LoadTexture("sky_bottom", "assets/tex/sky/bottom.png"), 
                                            texture_handler->GetTexture("sky_side2"), 
                                            texture_handler->GetTexture("sky_side2")
                                          };

    std::shared_ptr<Skybox> skybox = std::make_shared<Skybox>(skybox_textures);
    demo_scene->AddItem(std::make_shared<WE::RenderItem>("skybox", WE::RENDERITEM_TYPE::SKYBOX, shader_handler.GetProgram("sky"), skybox));

    SDL_Delay(1500);

    while (state_handler.GetState() != WE::STATE::EXIT) {
        window.UpdateDeltaTime();

        // ===============================
        // EDITOR
        // ===============================
        if (state_handler.GetState() == WE::STATE::EDITOR) {
            if (state_handler.Load()) {
                renderer.Clear();
                renderer.AddScene(demo_scene);
                demo_scene->Reload();
                renderer.Build(shader_handler.GetProgram("shadow"));

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
                        if (e.key.scancode == SDL_SCANCODE_LEFT) ball->ApplyImpulse(glm::vec3(-100.0f, 0.0f, 0.0f));
                        if (e.key.scancode == SDL_SCANCODE_RIGHT) ball->ApplyImpulse(glm::vec3(100.0f, 0.0f, 0.0f));
                        if (e.key.scancode == SDL_SCANCODE_UP) ball->ApplyImpulse(glm::vec3(0.0f, 100.0f, 0.0f));
                        if (e.key.scancode == SDL_SCANCODE_DOWN) ball->ApplyImpulse(glm::vec3(0.0f, -100.0f, 0.0f));

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
                        if (demo_scene->Raycast(camera.GetForwardRay(), hit)) {
                            ball->ApplyImpulse(-hit.normal*25.0f);
                            window.NeedRender();
                        }
                        break;
                }
            }

            if (camera.ProcessKey()) window.NeedRender();

            if (demo_scene->ProcessPhysics(*window.delta_time)) {
                demo_scene->ProcessCollisions(*window.delta_time);
                demo_scene->ApplyPhysics();
                window.NeedRender();
            }

            if (window.StartRender()) {
                renderer.RenderAll(camera.GetViewMatrix(), camera.GetProjectionMatrix(), camera.GetViewProjectionMatrix(), camera.GetPosition());
                window.EndRender();
            }
        }
    }

    // ===============================
    // EXIT + CLEAN UP
    // ===============================

    renderer.Clear();

    demo_scene->Destroy();

    texture_handler->Destroy();
    shader_handler.Destroy();

    SDL_Quit();

    return 0;
}
    