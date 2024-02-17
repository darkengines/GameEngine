#pragma once

#include <vector>
#include <string>
#include "./VectorKey.hpp"
#include "./QuatKey.hpp"
#include "./AnimationBehavior.hpp"
#include <entt/entt.hpp>

namespace drk::Animations::Components {
	struct NodeAnimation {
		entt::entity nodeEntity;
		entt::entity animationEntity;
		AnimationBehavior preState;
		AnimationBehavior postState;
		std::vector<VectorKey> positionKeys;
		std::vector<VectorKey> scalingKeys;
		std::vector<QuatKey> rotationKeys;
	};
}