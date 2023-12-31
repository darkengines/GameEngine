#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "../Models/Plane.hpp"

namespace drk::Planes::Components {
	struct Plane {
		glm::vec4 point;
		glm::vec4 normal;

		glm::vec4 getClosestPointToOrigin() const;
		float getDistanceToOrigin() const;
		float getSignedDistanceToPlan(const glm::vec4& center) const;
		operator Models::Plane() const;
	};
}