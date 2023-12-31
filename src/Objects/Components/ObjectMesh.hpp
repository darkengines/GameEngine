#pragma once
#include <entt/entt.hpp>

namespace drk::Objects::Components {
	struct ObjectMesh {
		entt::entity objectEntity;
		entt::entity meshEntity;
	};
}