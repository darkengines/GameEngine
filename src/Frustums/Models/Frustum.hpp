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
	};
}