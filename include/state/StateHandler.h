#ifndef SRC_STATEHANDLER_H_
#define SRC_STATEHANDLER_H_

#include <iostream>

#include <memory>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

class StateHandler {
public:
    StateHandler();
    ~StateHandler();

    void SetState(WE_STATE state);

    WE_STATE GetState() { return *current; }; 

private:
    std::shared_ptr<WE_STATE> current = std::make_shared<WE_STATE>(WE_STATE::BOOT);

};

#endif // SRC_STATEHANDLER_H_