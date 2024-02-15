#pragma once

#include <vector>
#include "entt/entt.hpp"

namespace drk::Nodes::Components {
	struct NodeMeshCollection {
		std::vector<entt::entity> nodeMeshes;
	};
}