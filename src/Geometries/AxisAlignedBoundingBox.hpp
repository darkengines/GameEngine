#pragma once

#include <glm/glm.hpp>
#include "Plane.hpp"
#include "Frustum.hpp"

namespace drk::Geometries {
	struct AxisAlignedBoundingBox {
		glm::vec4 center;
		glm::vec4 extent;

		static AxisAlignedBoundingBox fromMinMax(const glm::vec4 &min, const glm::vec4 &max);

		AxisAlignedBoundingBox transform(const glm::mat4 &model) const;

		bool isOnOrForwardPlane(const Plane &plane, const glm::mat4 &model) const;

		bool isOnOrForwardPlane(const Plane &plane) const;

		bool isOnFrustum(const Frustum &frustum);

		bool isInRadius(const glm::vec4 &ballCenter, float ballRadius);
	};
}