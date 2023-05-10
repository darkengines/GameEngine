#pragma once

#include "glm/glm.hpp"

namespace drk::Cameras::Components {
	struct Camera {
		glm::mat4 perspective;
		glm::mat4 view;
		glm::vec4 relativePosition;
		glm::vec4 relativeFront;
		glm::vec4 relativeUp;
		glm::vec4 absolutePosition;
		glm::vec4 absoluteFront;
		glm::vec4 absoluteUp;
		float verticalFov;
		float aspectRatio;
		float near;
		float far;
	};
}