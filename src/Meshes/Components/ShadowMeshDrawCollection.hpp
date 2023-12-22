#include <vector>
#include <entt/entt.hpp>

namespace drk::Meshes::Components {
	struct ShadowMeshDrawCollection {
		std::vector<entt::entity> meshDrawEntities;
	};
}