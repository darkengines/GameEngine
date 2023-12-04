
#pragma once

#include <glm/glm.hpp>
#include <optional>

namespace drk::Lights::Components {
	struct DirectionalLight {
		glm::mat4 perspective;
		glm::mat4 view;
		glm::vec4 relativeDirection;
		glm::vec4 relativeUp;
		glm::vec4 absoluteDirection;
		glm::vec4 absoluteUp;
		std::optional<glm::vec4> shadowMapRect;
	};
}