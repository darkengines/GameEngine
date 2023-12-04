#pragma once

#include "../../Common/ComponentIndex.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Engine::Models {
	struct Global {
		alignas(8) Stores::Models::StoreItemLocation cameraItemLocation;
		alignas(8) uint32_t  pointLightArrayIndex;
		alignas(8) uint32_t pointLightCount;
		alignas(8) uint32_t  directionalLightArrayIndex;
		alignas(8) uint32_t directionalLightCount;
		alignas(8) uint32_t  spotlightArrayIndex;
		alignas(8) uint32_t spotlightCount;
	};
}