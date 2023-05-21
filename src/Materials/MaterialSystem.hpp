#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "../Engine/EngineState.hpp"
#include "../Devices/Device.hpp"
#include "../Devices/DeviceContext.hpp"
#include "Models/Material.hpp"
#include "../Systems/System.hpp"

namespace drk::Materials {
	class MaterialSystem : public Systems::System<Models::Material, std::shared_ptr<Components::Material>> {
	protected:
		const Devices::DeviceContext& deviceContext;
	public:
		void Update(Models::Material& model, const std::shared_ptr<Components::Material>& components);
	public:
		MaterialSystem(
			const drk::Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			Engine::EngineState& engineState
		);
		static void AddMaterialSystem(entt::registry& registry);
		static void RemoveMaterialSystem(entt::registry& registry);
		static void OnMaterialConstruct(entt::registry& registry, entt::entity materialEntity);
		static entt::entity copyMaterialEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity);

	};
}