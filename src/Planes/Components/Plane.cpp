#include "Plane.hpp"

namespace drk::Planes::Components {

	float Plane::getDistanceToOrigin() const {
		return glm::dot(point, normal);
	}

	float Plane::getSignedDistanceToPlan(const glm::vec4& center) const {
		return glm::dot(center, normal) - getDistanceToOrigin();
	}

	glm::vec4 Plane::getClosestPointToOrigin() const {
		return glm::dot(point, normal) * normal;
	}
	Plane::operator Models::Plane() const {
		return {
			.point = point,
			.normal = normal
		};
	}
}