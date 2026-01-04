#include "state/StateHandler.h"

StateHandler::StateHandler() {
    ;
}

StateHandler::~StateHandler() {
    ;
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