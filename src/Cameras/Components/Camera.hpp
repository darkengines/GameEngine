#pragma once

#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace drk::Cameras::Components {
	struct Camera {
		glm::mat4 perspective;
		glm::mat4 view;
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;
		
		float verticalFov;
		float aspectRatio;
		float near;
		float far;
	};
}