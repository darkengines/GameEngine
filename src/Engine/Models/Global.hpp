#pragma once

#include "../../Common/ComponentIndex.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Engine::Models {
	struct Global {
		uint32_t cameraStoreIndex;
		uint32_t cameraItemIndex;
		uint32_t pointLightArrayIndex;
		uint32_t pointLightCount;
		uint32_t directionalLightArrayIndex;
		uint32_t directionalLightCount;
		uint32_t spotlightArrayIndex;
		uint32_t spotlightCount;
		uint32_t renderStyle;
	};
}