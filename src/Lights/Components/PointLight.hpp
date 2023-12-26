#pragma once
#include <entt/entity/fwd.hpp>

namespace drk::Lights::Components {
	struct PointLight {
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;

		entt::entity frontLightPerspectiveEntity;
		entt::entity backLightPerspectiveEntity;
		entt::entity leftLightPerspectiveEntity;
		entt::entity rightLightPerspectiveEntity;
		entt::entity topLightPerspectiveEntity;
		entt::entity downLightPerspectiveEntity;
	};
}