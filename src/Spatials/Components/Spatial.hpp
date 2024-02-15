#pragma once

#include "glm/ext.hpp"
#include "glm/glm.hpp"

namespace drk::Spatials::Components {
	enum class SpatialType {
		Relative,
		Absolute
	};

	template<typename T>
	struct Spatial {
		glm::vec4 position;
		glm::quat rotation;
		glm::vec4 scale;
		glm::mat4 model;
	};
	struct Absolute {
	};
	struct Relative {
	};
}
