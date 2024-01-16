#pragma once

#include <boost/di.hpp>
#include "Systems/AnimationSystem.hpp"

namespace drk::Animations {
	auto AddAnimations() {
		return boost::di::make_injector(
			boost::di::bind<Systems::AnimationSystem>.to<Systems::AnimationSystem>()
		);
	}
}