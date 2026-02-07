#ifndef WE_STATE_STATEHANDLER_H_
#define WE_STATE_STATEHANDLER_H_

#include <iostream>
#include <unordered_map>

#include <memory>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "scene/Scene.h"

class StateHandler {
public:
    StateHandler();

    void Destroy();

    bool Load();
    void Reload();

    void SetState(WE::STATE state);
    WE::STATE GetState() { return *current; }; 

    void AddScene(std::string name, std::shared_ptr<Scene> scene);
    void SetScene(std::string name);
    std::shared_ptr<Scene> GetScene(std::string name);
    std::shared_ptr<Scene> GetCurrentScene() { return current_scene; };

private:
    std::shared_ptr<WE::STATE> current = std::make_shared<WE::STATE>(WE::STATE::BOOT);
    std::shared_ptr<Scene> current_scene = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes = {};

    bool initial_load = true;

};

#endif // WE_STATE_STATEHANDLER_H_