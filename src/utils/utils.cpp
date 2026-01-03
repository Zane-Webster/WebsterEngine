#include "utils/Utils.h"

const std::unordered_map<SDL_Scancode, glm::vec3> Utils::wasd_keyset_map = {
    { SDL_SCANCODE_W,      {  0,  0, 1 } },
    { SDL_SCANCODE_S,      {  0,  0,  -1 } },
    { SDL_SCANCODE_A,      { -1,  0,  0 } },
    { SDL_SCANCODE_D,      {  1,  0,  0 } },
    { SDL_SCANCODE_SPACE,  {  0,  1,  0 } },
    { SDL_SCANCODE_LCTRL,  {  0, -1,  0 } }
};

const std::unordered_map<SDL_Scancode, glm::vec3> Utils::arrows_keyset_map = {
    { SDL_SCANCODE_UP,     {  0,  0, -1 } },
    { SDL_SCANCODE_DOWN,   {  0,  0,  1 } },
    { SDL_SCANCODE_LEFT,   { -1,  0,  0 } },
    { SDL_SCANCODE_RIGHT,  {  1,  0,  0 } },
    { SDL_SCANCODE_SPACE,  {  0,  1,  0 } },
    { SDL_SCANCODE_LCTRL,  {  0, -1,  0 } }
};

glm::vec3 Utils::MovementFromScancode(WE::KEYSET keyset, SDL_Scancode scancode) {
    const auto& map =
        (keyset == WE::KEYSET::WASD)
            ? wasd_keyset_map
            : arrows_keyset_map;

    auto it = map.find(scancode);
    if (it != map.end()) return it->second;

    return glm::vec3(0.0f);
}