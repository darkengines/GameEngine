#pragma once
#include <vector>
#include <entt/entt.hpp>

namespace drk::Materials::Components {
	struct MaterialCollection {
		std::vector<entt::entity> materialEntities;
	};
}