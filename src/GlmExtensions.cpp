#include "GlmExtensions.hpp"

namespace drk {
	glm::vec4 GlmExtensions::cross(const glm::vec4 &left, const glm::vec4 &right) {
		return glm::vec4(glm::cross(glm::make_vec3(left), glm::make_vec3(right)), 0.0f);
	}
}