#pragma once
#include <glm/vec4.hpp>
#include <optional>
#include <glm/mat4x4.hpp>

namespace drk::Lights::Models {
	struct Spotlight {
		glm::vec4 relativePosition;
		glm::vec4 relativeDirection;
		glm::vec4 relativeUp;
		glm::vec4 absolutePosition;
		glm::vec4 absoluteDirection;
		glm::vec4 absoluteUp;
		std::optional<glm::vec4> shadowMapRect;
		glm::mat4 view;
		glm::mat4 perspective;
		float innerConeAngle;
		float outerConeAngle;
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
	};
}
