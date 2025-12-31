#include "state/StateHandler.h"

StateHandler::StateHandler() {
    ;
}

StateHandler::~StateHandler() {
    ;
}

void StateHandler::SetState(WE::STATE p_state) {
    *current = p_state;
}