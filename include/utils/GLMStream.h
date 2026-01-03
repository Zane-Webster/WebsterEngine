#pragma once

#include <ostream>
#include <glm/glm.hpp>

std::ostream& operator<<(std::ostream& os, const glm::vec2& v);
std::ostream& operator<<(std::ostream& os, const glm::vec3& v);
std::ostream& operator<<(std::ostream& os, const glm::vec4& v);

std::ostream& operator<<(std::ostream& os, const glm::mat3& m);
std::ostream& operator<<(std::ostream& os, const glm::mat4& m);