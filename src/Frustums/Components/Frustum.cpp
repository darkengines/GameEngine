#include "Frustum.hpp"
#include "../../GlmExtensions.hpp"
namespace drk::Frustums::Components {
	Frustum Frustum::createFrustumFromView(
		const glm::vec4& position,
		const glm::vec4& front,
		const glm::vec4& up,
		float verticalFov,
		float aspectRatio,
		float near,
		float far
	) {
		const auto farHalfHeight = glm::tan(verticalFov * 0.5f) * far;
		const auto farHalfWidth = aspectRatio * farHalfHeight;
		const auto nearHalfHeight = glm::tan(verticalFov * 0.5f) * near;
		const auto nearHalfWidth = aspectRatio * nearHalfHeight;
		const auto right = GlmExtensions::cross(up, front);
		const auto relativeNearCenter = front * near;
		const auto relativeFarCenter = front * far;
		const auto nearCenter = position + relativeNearCenter;
		const auto farCenter = position + relativeFarCenter;
		const auto relativeFarTop = up * farHalfHeight;
		const auto relativeFarRight = right * farHalfWidth;
		const auto relativeNearTop = up * nearHalfHeight;
		const auto relativeNearRight = right * nearHalfWidth;

		Planes::Components::Plane farPlane{ farCenter, glm::normalize(-front) };
		Planes::Components::Plane nearPlane{ nearCenter, glm::normalize(front) };
		Planes::Components::Plane topPlane{ position, glm::normalize(GlmExtensions::cross(right, relativeFarCenter + relativeFarTop)) };
		Planes::Components::Plane bottomPlane{ position, glm::normalize(GlmExtensions::cross(relativeFarCenter - relativeFarTop, right)) };
		Planes::Components::Plane leftPlane{ position, glm::normalize(GlmExtensions::cross(relativeFarCenter + relativeFarRight, up)) };
		Planes::Components::Plane rightPlane{ position, glm::normalize(GlmExtensions::cross(up, relativeFarCenter - relativeFarRight)) };

		auto rightTopFar = farCenter + relativeFarTop + relativeFarRight;
		auto leftTopFar = farCenter + relativeFarTop - relativeFarRight;
		auto rightBottomFar = farCenter - relativeFarTop + relativeFarRight;
		auto leftBottomFar = farCenter - relativeFarTop - relativeFarRight;
		auto rightTopNear = nearCenter + relativeNearTop + relativeNearRight;
		auto leftTopNear = nearCenter + relativeNearTop - relativeNearRight;
		auto rightBottomNear = nearCenter - relativeNearTop + relativeNearRight;
		auto leftBottomNear = nearCenter - relativeNearTop - relativeNearRight;

		return {
			.nearPlane = nearPlane,
			.farPlane = farPlane,
			.leftPlane = leftPlane,
			.rightPlane = rightPlane,
			.topPlane = topPlane,
			.bottomPlane = bottomPlane,
			.rightTopFar = rightTopFar,
			.leftTopFar = leftTopFar,
			.rightBottomFar = rightBottomFar,
			.leftBottomFar = leftBottomFar,
			.rightTopNear = rightTopNear,
			.leftTopNear = leftTopNear,
			.rightBottomNear = rightBottomNear,
			.leftBottomNear = leftBottomNear
		};
	}
}