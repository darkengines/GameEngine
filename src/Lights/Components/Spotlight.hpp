#pragma once

namespace drk::Lights::Components {
	struct Spotlight {
		float innerConeAngle;
		float outerConeAngle;
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
	};
}
