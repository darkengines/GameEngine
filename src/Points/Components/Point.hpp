#pragma once

#include <entt/entt.hpp>
#include <vector>

namespace drk::Points::Components {
	struct Point {
		entt::entity materialEntity;
		std::shared_ptr<Materials::Components::Material> pMaterial;
	};
}