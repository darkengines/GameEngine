#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk::Animations::Models {
	struct BoneSpatial {
		glm::vec4 position;
		glm::quat rotation;
		glm::vec4 scale;
		glm::mat4 model;
	};
}