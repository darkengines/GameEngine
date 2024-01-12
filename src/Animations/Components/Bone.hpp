#include <vector>
#include <glm/glm.hpp>
#include "../../Spatials/Components/Spatial.hpp"
#include <entt/entt.hpp>

namespace drk::Animations::Components {
	struct Bone {
		std::vector<entt::entity> weightEntities;
		Spatials::Components::Spatial spatialOffset;
	};
}