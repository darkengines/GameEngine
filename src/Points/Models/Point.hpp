#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Points::Models {
	struct Point {
		Stores::Models::StoreItemLocation materialItemLocation;
	};
}