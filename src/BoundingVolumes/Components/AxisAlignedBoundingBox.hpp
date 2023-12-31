#pragma once

#include <glm/glm.hpp>
#include "../../Planes/Components/Plane.hpp"
#include "../../Frustums/Components/Frustum.hpp"

namespace drk::BoundingVolumes::Components {
	struct AxisAlignedBoundingBox {
		glm::vec4 center;
		glm::vec4 extent;

		static AxisAlignedBoundingBox fromMinMax(const glm::vec4& min, const glm::vec4& max);

		AxisAlignedBoundingBox transform(const glm::mat4& model) const;

		bool isOnOrForwardPlane(const Planes::Components::Plane& plane, const glm::mat4& model) const;

		bool isOnOrForwardPlane(const Planes::Components::Plane& plane) const;

		bool isOnFrustum(const Frustums::Components::Frustum& frustum);

		bool isInRadius(const glm::vec4& ballCenter, float ballRadius);
	};
}