#pragma once

#include <vector>
#include <entt/entity/entity.hpp>

namespace drk::Meshes {
	struct MeshGroup {
		std::vector<entt::entity> meshEntities;
	};
}