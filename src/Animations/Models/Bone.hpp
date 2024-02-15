#pragma once
#include <vector>
#include "./VertexWeight.hpp"
#include <glm/glm.hpp>
#include "../../Spatials/Components/Spatial.hpp"

namespace drk::Animations::Models {
	struct Bone {
		Spatials::Components::Spatial <Spatials::Components::Relative> spatialOffset;
	};
}