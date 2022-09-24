#pragma once

#include <glm/glm.hpp>

namespace drk::Geometries {
	struct Plane {
		glm::vec4 point;
		glm::vec4 normal;

		glm::vec4 getClosestPointToOrigin() const;

		float getDistanceToOrigin() const;

		float getSignedDistanceToPlan(const glm::vec4 &center) const;
	};
}
