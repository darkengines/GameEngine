
#pragma once

#include <glm/glm.hpp>
#include <optional>
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Lights::Models {
	struct DirectionalLight {
		glm::mat4 perspective;
		glm::mat4 view;
		glm::vec4 relativeDirection;
		glm::vec4 relativeUp;
		glm::vec4 absoluteDirection;
		glm::vec4 absoluteUp;
		glm::vec4 shadowMapRect;
		Stores::Models::StoreItemLocation lightStoreItemLocation;
		Stores::Models::StoreItemLocation spatialStoreItemLocation;
	};
}