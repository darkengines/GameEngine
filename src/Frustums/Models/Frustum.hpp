#pragma once

#include <glm/glm.hpp>
#include "../../Planes/Models/Plane.hpp"

namespace drk::Frustums::Models {
	struct Frustum {
		Planes::Models::Plane nearPlane;
		Planes::Models::Plane farPlane;
		Planes::Models::Plane leftPlane;
		Planes::Models::Plane rightPlane;
		Planes::Models::Plane topPlane;
		Planes::Models::Plane bottomPlane;
		glm::vec4 rightTopFar;
		glm::vec4 leftTopFar;
		glm::vec4 rightBottomFar;
		glm::vec4 leftBottomFar;
		glm::vec4 rightTopNear;
		glm::vec4 leftTopNear;
		glm::vec4 rightBottomNear;
		glm::vec4 leftBottomNear;
	};
}