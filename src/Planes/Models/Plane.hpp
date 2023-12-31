#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace drk::Planes::Models {
	struct Plane {
		glm::vec4 point;
		glm::vec4 normal;
	};
}