#pragma once

#include "Plane.hpp"
#include <glm/glm.hpp>

namespace drk::Geometries {
	struct Frustum {
		Plane nearPlane;
		Plane farPlane;
		Plane leftPlane;
		Plane rightPlane;
		Plane topPlane;
		Plane bottomPlane;

		glm::vec4 rightTopFar;
		glm::vec4 leftTopFar;
		glm::vec4 rightBottomFar;
		glm::vec4 leftBottomFar;
		glm::vec4 rightTopNear;
		glm::vec4 leftTopNear;
		glm::vec4 rightBottomNear;
		glm::vec4 leftBottomNear;

		static Frustum createFrustumFromView(const glm::vec4& position, const glm::vec4& front, const glm::vec4& up, float verticalFov, float aspectRatio, float near, float far);
	};
}