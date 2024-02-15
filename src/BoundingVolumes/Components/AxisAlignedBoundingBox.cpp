#include "AxisAlignedBoundingBox.hpp"
#include "../../GlmExtensions.hpp"

namespace drk::BoundingVolumes::Components {

	AxisAlignedBoundingBox AxisAlignedBoundingBox::fromMinMax(const glm::vec4& min, const glm::vec4& max) {
		auto center = (min + max) / 2.0f;
		auto extent = max - center;

		return {
			.center = center,
			.extent = extent
		};
	}

	AxisAlignedBoundingBox AxisAlignedBoundingBox::transform(const glm::mat4& model) const {

		glm::vec4 right = model * GlmExtensions::right * extent.x;
		glm::vec4 top = model * GlmExtensions::up * extent.y;
		glm::vec4 forward = model * GlmExtensions::front * extent.z;

		auto i = abs(dot(GlmExtensions::right, right)) + abs(dot(GlmExtensions::right, top)) +
				 abs(dot(GlmExtensions::right, forward));
		auto j = abs(dot(GlmExtensions::up, right)) + abs(dot(GlmExtensions::up, top)) +
				 abs(dot(GlmExtensions::up, forward));
		auto k = abs(dot(GlmExtensions::front, right)) + abs(dot(GlmExtensions::front, top)) +
				 abs(dot(GlmExtensions::front, forward));

		glm::vec4 newExtent{i, j, k, 0};
		glm::vec4 newCenter = model * center;

		return {newCenter, newExtent};
	}

	void AxisAlignedBoundingBox::inplaceTransform(const glm::mat4& model) {

		glm::vec4 right = model * GlmExtensions::right * extent.x;
		glm::vec4 top = model * GlmExtensions::up * extent.y;
		glm::vec4 forward = model * GlmExtensions::front * extent.z;

		absoluteExtent.x = abs(dot(GlmExtensions::right, right)) + abs(dot(GlmExtensions::right, top)) +
						   abs(dot(GlmExtensions::right, forward));
		absoluteExtent.y = abs(dot(GlmExtensions::up, right)) + abs(dot(GlmExtensions::up, top)) +
						   abs(dot(GlmExtensions::up, forward));
		absoluteExtent.z = abs(dot(GlmExtensions::front, right)) + abs(dot(GlmExtensions::front, top)) +
						   abs(dot(GlmExtensions::front, forward));
		absoluteCenter = model * center;
	}

	bool
	AxisAlignedBoundingBox::isOnOrForwardPlane(const Planes::Components::Plane& plane, const glm::mat4& model) const {
		auto globalAABB = transform(model);
		auto radius =
			globalAABB.extent.x * glm::abs(plane.normal.x) + globalAABB.extent.y * glm::abs(plane.normal.y) +
			globalAABB.extent.z * glm::abs(plane.normal.z);
		auto signedDistance = plane.getSignedDistanceToPlan(globalAABB.center);
		return signedDistance >= -radius;
	}

	bool AxisAlignedBoundingBox::isOnOrForwardPlane(const Planes::Components::Plane& plane) const {
		auto radius = extent.x * glm::abs(plane.normal.x) + extent.y * glm::abs(plane.normal.y) +
					  extent.z * glm::abs(plane.normal.z);
		auto signedDistance = plane.getSignedDistanceToPlan(center);
		return signedDistance >= -radius;
	}

	bool AxisAlignedBoundingBox::isOnFrustum(const Frustums::Components::Frustum& frustum) {
		return isOnOrForwardPlane(frustum.nearPlane)
			   && isOnOrForwardPlane(frustum.leftPlane)
			   && isOnOrForwardPlane(frustum.rightPlane)
			   && isOnOrForwardPlane(frustum.bottomPlane)
			   && isOnOrForwardPlane(frustum.topPlane)
			   && isOnOrForwardPlane(frustum.farPlane);
	}

//Todo: implementation
	bool AxisAlignedBoundingBox::isInRadius(const glm::vec4& ballCenter, float ballRadius) {
		auto ballToCenter = center - ballCenter;
		auto ballToCenterDistance = glm::length(ballToCenter);
		if (ballToCenterDistance < ballRadius) return true;
		return true;
	}
}