#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../../Spatials/Components/Spatial.hpp"
#include "./VertexWeight.hpp"

namespace drk::Animations::Components {
	struct Bone {
		Spatials::Components::Spatial<Spatials::Components::Relative> spatialOffset;
	};
}