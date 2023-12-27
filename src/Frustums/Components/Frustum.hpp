#include <glm/glm.hpp>

namespace drk::Frustums::Components {
	struct Frustum {
		glm::vec4 relativeFront;
		glm::vec4 absoluteFront;
		glm::vec4 relativeUp;
		glm::vec4 absoluteUp;
		float verticalFov;
		float aspectRatio;
		float near;
		float far;
	};
}