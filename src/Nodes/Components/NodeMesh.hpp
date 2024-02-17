#pragma once
#include <entt/entt.hpp>

namespace drk::Nodes::Components {
	struct NodeMesh {
		entt::entity nodeEntity;
		entt::entity meshEntity;
	};
}