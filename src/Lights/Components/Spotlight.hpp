#pragma once
#include <entt/entity/fwd.hpp>

namespace drk::Lights::Components {
	struct Spotlight {
		float innerConeAngle;
		float outerConeAngle;
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
	};
}
