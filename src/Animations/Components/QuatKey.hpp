#pragma once
#include <glm/ext.hpp>

namespace drk::Animations::Components {
	struct QuatKey {
		double time;
		glm::quat quat;
	};
}