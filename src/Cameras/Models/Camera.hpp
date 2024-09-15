#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk::Cameras::Models {
	struct Camera {
		glm::mat4 perspective;
		glm::mat4 view;
		glm::vec4 position;
		glm::vec4 front;
		glm::vec4 up;
		float verticalFov;
		float aspectRatio;
		float near;
		float far;
	};
}
