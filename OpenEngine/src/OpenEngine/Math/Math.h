#pragma once

#include <random>
#include <glm/glm.hpp>

namespace OpenEngine { namespace Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranlation, glm::vec3& outRotaion, glm::vec3& outScale);

} }