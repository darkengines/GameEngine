#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "../Graphics/EngineState.hpp"
#include "../Devices/Device.hpp"
#include "Models/Material.hpp"

namespace drk::Materials {
	class MaterialSystem {
	protected:
		Devices::DeviceContext *DeviceContext;
		Graphics::EngineState *EngineState;
		void UpdateStoreItem(const Material *material, Models::Material &storedMaterial);
	public:
		MaterialSystem(Devices::DeviceContext *pContext, Graphics::EngineState *pState);
		void UpdateMaterials();
		void StoreMaterials();
	};
}