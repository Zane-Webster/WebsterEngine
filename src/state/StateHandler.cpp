#include "state/StateHandler.h"

StateHandler::StateHandler() {
    ;
}

StateHandler::~StateHandler() {
    ;
}

void StateHandler::SetState(WE_STATE p_state) {
    *current = p_state;
}