
#pragma once

#include <glm/gtx/quaternion.hpp>
#include <imgui.h>
#include <vector>

#include "Spatial.hpp"

namespace drk::Spatials::Components {
class SpatialEditor {
public:
	static bool Spatial(Spatial<Relative>& relativeSpatial, const Spatial<Absolute>& absoluteSpatial);
};
}