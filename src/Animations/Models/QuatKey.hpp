#pragma once
#include <glm/ext.hpp>

namespace drk::Animations::Models {
	struct QuatKey {
		double time;
		glm::quat quat;
	};
}