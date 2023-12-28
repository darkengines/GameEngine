#pragma once

#include <vector>
#include "entt/entity/entity.hpp"

namespace drk::Meshes::Components {
	struct MeshGroup {
		std::vector<entt::entity> meshEntities;
	};
}