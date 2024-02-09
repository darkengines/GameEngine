#pragma once

#include <glm/vec4.hpp>
#include <glm/ext.hpp>

namespace drk::Spatials::Models {
	struct RelativeSpatial {
		glm::vec4 position;
		glm::quat rotation;
		glm::vec4 scale;
		glm::mat4 model;
	};
}