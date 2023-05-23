#pragma once

#include <entt/entity/entity.hpp>

namespace drk::Objects {
	struct Relationship {
		entt::entity parent{entt::null};
		std::vector<entt::entity> children;
		int depth;
	};

}