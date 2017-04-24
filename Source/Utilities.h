#pragma once
#include <glm/vec3.hpp>
#include <ostream>

inline std::ostream & operator<<(std::ostream & os, const glm::vec3 & v)
{
	return (os << v.x << " " << v.y << " " << v.z);
}