#pragma once
#include <vector>
#include <entt/entt.hpp>

namespace drk::Animations::Components {
	struct Bone {
		std::vector<entt::entity> boneMeshEntities;
	};
}