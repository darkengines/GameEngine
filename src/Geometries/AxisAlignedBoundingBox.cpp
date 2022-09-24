#include "AxisAlignedBoundingBox.hpp"

namespace drk::Geometries {

	AxisAlignedBoundingBox AxisAlignedBoundingBox::fromMinMax(const glm::vec4 &min, const glm::vec4 &max) {
		auto center = (min + max) / 2.0f;
		auto extent = max - center;

		return {
			.center = center,
			.extent = extent
		};
	}

	AxisAlignedBoundingBox AxisAlignedBoundingBox::transform(const glm::mat4 &model) const {
		glm::vec4 xAxis{1.0, 0.0, 0.0, 0.0};
		glm::vec4 yAxis{0.0, 1.0, 0.0, 0.0};
		glm::vec4 zAxis{0.0, 0.0, 1.0, 0.0};

		glm::vec4 right = model * xAxis * extent.x;
		glm::vec4 top = model * yAxis * extent.y;
		glm::vec4 forward = model * zAxis * extent.z;

		auto i = abs(dot(xAxis, right)) + abs(dot(xAxis, top)) + abs(dot(xAxis, forward));
		auto j = abs(dot(yAxis, right)) + abs(dot(yAxis, top)) + abs(dot(yAxis, forward));
		auto k = abs(dot(zAxis, right)) + abs(dot(zAxis, top)) + abs(dot(zAxis, forward));

		glm::vec4 newExtent{i, j, k, 0};
		glm::vec4 newCenter = model * center;

		return {newCenter, newExtent};
	}

	bool AxisAlignedBoundingBox::isOnOrForwardPlane(const Plane &plane, const glm::mat4 &model) const {
		auto globalAABB = transform(model);
		auto radius =
			globalAABB.extent.x * glm::abs(plane.normal.x) + globalAABB.extent.y * glm::abs(plane.normal.y) +
			globalAABB.extent.z * glm::abs(plane.normal.z);
		auto signedDistance = plane.getSignedDistanceToPlan(globalAABB.center);
		return signedDistance >= -radius;
	}

	bool AxisAlignedBoundingBox::isOnOrForwardPlane(const Plane &plane) const {
		auto radius = extent.x * glm::abs(plane.normal.x) + extent.y * glm::abs(plane.normal.y) +
					  extent.z * glm::abs(plane.normal.z);
		auto signedDistance = plane.getSignedDistanceToPlan(center);
		return signedDistance >= -radius;
	}

	bool AxisAlignedBoundingBox::isOnFrustum(const Frustum &frustum) {
		return isOnOrForwardPlane(frustum.nearPlane)
			   && isOnOrForwardPlane(frustum.leftPlane)
			   && isOnOrForwardPlane(frustum.rightPlane)
			   && isOnOrForwardPlane(frustum.bottomPlane)
			   && isOnOrForwardPlane(frustum.topPlane)
			   && isOnOrForwardPlane(frustum.farPlane);
	}

	//Todo: implementation
	bool AxisAlignedBoundingBox::isInRadius(const glm::vec4 &ballCenter, float ballRadius) {
		auto ballToCenter = center - ballCenter;
		auto ballToCenterDistance = glm::length(ballToCenter);
		if (ballToCenterDistance < ballRadius) return true;
		return true;
	}
}
