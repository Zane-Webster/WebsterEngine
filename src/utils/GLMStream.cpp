#include "utils/GlmStream.h"

std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}

std::ostream& operator<<(std::ostream& os, const glm::mat3& m) {
    for (int i = 0; i < 3; ++i)
        os << "[" << m[i][0] << " " << m[i][1] << " " << m[i][2] << "]\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::mat4& m) {
    for (int i = 0; i < 4; ++i)
        os << "[" << m[i][0] << " " << m[i][1] << " "
           << m[i][2] << " " << m[i][3] << "]\n";
    return os;
}
