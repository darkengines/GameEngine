#include <vector>
#include "./LightPerspective.hpp"
#include <entt/entt.hpp>

namespace drk::Lights::Components {
	struct LightPerspectiveCollection {
		std::vector<entt::entity> lightPerspectives
	};
}