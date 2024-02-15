#pragma once

#include <vector>
#include "entt/entity/entity.hpp"

namespace drk::Nodes::Components {
	struct Node {
		entt::entity parent{entt::null};
		std::vector<entt::entity> children;
		int depth;
	};

}