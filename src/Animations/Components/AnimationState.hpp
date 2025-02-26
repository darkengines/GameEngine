#pragma once
#include "./AnimationBehavior.hpp"
#include "./Animation.hpp"
#include "./NodeAnimation.hpp"
#include <entt/entt.hpp>

namespace drk::Animations::Components {
	struct AnimationState {
		double progress;
	};
}
