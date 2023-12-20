#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace drk::Lights::Components {
	struct LightPerspective {
		glm::mat4 perspective;
		glm::mat4 view;
		glm::vec4 relativeFront;
		glm::vec4 relativeUp;
		glm::vec4 absoluteFront;
		glm::vec4 absoluteUp;
		vk::Rect2D shadowMapRect;
		float verticalFov;
		float aspectRatio;
		float near;
		float far;
	};
}