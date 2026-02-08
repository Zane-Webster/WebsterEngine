#include "state/StateHandler.h"

StateHandler::StateHandler() {
    ;
}

void StateHandler::Destroy() {
    for (auto& [name, scene] : scenes) {
        scene->Destroy();
    }
    scenes = {};
}

bool StateHandler::Load() {
    if (initial_load) {
        initial_load = false;
        return true;
    }
    return false;
}

void StateHandler::Reload() {
    initial_load = true;
}

void StateHandler::SetState(WE::STATE p_state) {
    *current = p_state;
    initial_load = true;
}

void StateHandler::AddScene(std::string name, std::shared_ptr<Scene> scene) {
    if (scenes.contains(name)) {
        Logger::Error("[StateHandler::AddScene] SCENE: " + name + " ALREADY EXISTS");
        return;
    }
    scenes.emplace(name, scene);
}

void StateHandler::SetScene(std::string name) {
     if (!scenes.contains(name)) {
        Logger::Error("[StateHandler::SetScene] SCENE: " + name + " DOES NOT EXIST");
        return;
    }
    current_scene = scenes[name];
}

std::shared_ptr<Scene> StateHandler::GetScene(std::string name) {
     if (!scenes.contains(name)) {
        Logger::Error("[StateHandler::GetScene] SCENE: " + name + " DOES NOT EXIST");
        return nullptr;
    }
    return scenes[name];
}