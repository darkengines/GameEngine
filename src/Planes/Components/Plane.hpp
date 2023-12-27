#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace drk::Planes::Components {
	struct Plane {
		entt::entity materialEntity;
		glm::vec4 normal;
	};
}