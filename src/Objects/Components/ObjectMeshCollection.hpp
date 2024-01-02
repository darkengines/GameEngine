#pragma once

#include <vector>
#include <entt/entt.hpp>

namespace drk::Objects::Components {
	struct ObjectMeshCollection
	{
		std::vector<entt::entity> objectMeshes;
	};
}