#ifndef WE_STATE_STATEHANDLER_H_
#define WE_STATE_STATEHANDLER_H_

#include <iostream>

#include <memory>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

class StateHandler {
public:
    StateHandler();
    ~StateHandler();

    bool Load();

    void SetState(WE::STATE state);

    WE::STATE GetState() { return *current; }; 

private:
    std::shared_ptr<WE::STATE> current = std::make_shared<WE::STATE>(WE::STATE::BOOT);
    bool initial_load = true;

};

#endif // WE_STATE_STATEHANDLER_H_