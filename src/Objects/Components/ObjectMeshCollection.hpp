#pragma once

#include <vector>
#include <entt/entt.hpp>

namespace drk::Nodes::Components {
	struct ObjectMeshCollection {
		std::vector<entt::entity> objectMeshes;
	};
}