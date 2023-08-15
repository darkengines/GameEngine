#include <vector>
#include <entt/entt.hpp>

namespace drk::Meshes::Components {
	struct MeshDrawCollection {
		std::vector<entt::entity> meshDrawEntities;
	};
}