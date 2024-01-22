#pragma once
#include "./AnimationBehavior.hpp"
#include "./Animation.hpp"
#include "./NodeAnimation.hpp"
#include <chrono>
#include <entt/entt.hpp>

namespace drk::Animations::Components {
	struct AnimationState {
		double progress;
	};
}