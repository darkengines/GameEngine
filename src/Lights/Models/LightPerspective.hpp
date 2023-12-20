#include <glm/glm.hpp>

namespace drk::Lights::Models {
	struct LightPerspective {
		glm::mat4 perspective;
		glm::mat4 view;
		glm::vec4 relativeFront;
		glm::vec4 relativeUp;
		glm::vec4 absoluteFront;
		glm::vec4 absoluteUp;
		glm::vec4 shadowMapRect;
		float verticalFov;
		float aspectRatio;
		float near;
		float far;
	};
}