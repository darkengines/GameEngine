#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "../Graphics/EngineState.hpp"
#include "../Devices/Device.hpp"
#include "../Devices/DeviceContext.hpp"
#include "Models/Material.hpp"

namespace drk::Materials {
	class MaterialSystem {
	protected:
		const Devices::DeviceContext& DeviceContext;
		entt::registry& Registry;
		Graphics::EngineState& EngineState;
		void UpdateStoreItem(const Material* material, Models::Material& storedMaterial);
	public:
		MaterialSystem(
			const drk::Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			Graphics::EngineState& engineState
		);
		static void AddMaterialSystem(entt::registry& registry);
		static void RemoveMaterialSystem(entt::registry& registry);
		static void OnMaterialConstruct(entt::registry& registry, entt::entity materialEntity);
		void UpdateMaterials();
		void StoreMaterials();
	};
}