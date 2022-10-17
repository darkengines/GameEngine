#pragma once

#include <glm/vec4.hpp>
#include <glm/ext.hpp>

namespace drk::Spatials::Models {
	struct Spatial {
		glm::vec4 relativeScale;
		glm::quat relativeRotation;
		glm::vec4 relativePosition;
		glm::vec4 absoluteScale;
		glm::quat absoluteRotation;
		glm::vec4 absolutePosition;
		glm::mat4 relativeModel;
		glm::mat4 absoluteModel;
	};
}