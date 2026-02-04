#ifndef WE_UTILS_UTILS_H_
#define WE_UTILS_UTILS_H_

#include <iostream>

#include <vector>
#include <algorithm>
#include <unordered_map>

#include "SDL3/SDL.h"
#include "glm/glm.hpp"

#include "core/WebsterEngine.h"

class Utils {
public:
    template<typename T>
    static bool VectorContains(const std::vector<T>& vec, const T& value) { return std::find(vec.begin(), vec.end(), value) != vec.end(); };

    static glm::vec3 MovementFromScancode(WE::KEYSET keyset, SDL_Scancode scancode);

    static std::string WETypeToString(WE::RENDERITEM_TYPE type);
    static WE::COLLIDER_TYPE ColliderTypeFromString(std::string str);

    static bool RayIntersectsAABB(WE::Ray ray, WE::AABB box, float& t);
    static bool RayIntersectsTriangle(WE::Ray ray, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float& t);

private:
    static const std::unordered_map<SDL_Scancode, glm::vec3> wasd_keyset_map;
    static const std::unordered_map<SDL_Scancode, glm::vec3> arrows_keyset_map;
};

#endif // WE_UTILS_UTILS_H_