#pragma once

#include <glm/mat4x4.hpp>
#include <optional>

namespace drk::Lights {
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