#include <vector>
#include <entt/entt.hpp>

namespace drk::Animations::Components {
	struct MeshBoneCollection {
		std::vector<entt::entity> meshBoneEntities;
	};
}