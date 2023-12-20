#pragma once

namespace drk::Lights::Components {
	struct PointLight {
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
	};
}