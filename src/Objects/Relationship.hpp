#pragma once

#include <entt/entity/entity.hpp>

namespace drk::Objects {
	struct Relationship {
		size_t childCount{0};
		entt::entity firstChild{entt::null};
		entt::entity previousSibling{entt::null};
		entt::entity nextSibling{entt::null};
		entt::entity parent{entt::null};
		std::vector<entt::entity> children;
		int depth;
	};

}