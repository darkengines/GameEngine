#include <entt/entt.hpp>

namespace drk::Animations::Components {
	struct AnimationState {
		entt::entity animationEntity;
		double progress;
	};
}