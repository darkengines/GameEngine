#pragma once
#include <glm/glm.hpp>

namespace drk::Animations::Components {
	struct VectorKey {
		double time;
		glm::vec3 vector;
	};
}