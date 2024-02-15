#pragma once
#include <vulkan/vulkan.hpp>
#include "../../Engine/EngineState.hpp"
#include "../../Devices/Device.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../Models/Material.hpp"
#include "../../Systems/System.hpp"

namespace drk::Materials::Systems {
	class MaterialSystem : public drk::Systems::System<Models::Material, std::shared_ptr<Components::Material>> {
	protected:
		const Devices::DeviceContext& deviceContext;
	public:
		void update(Models::Material& model, const std::shared_ptr<Components::Material>& components);
	public:
		MaterialSystem(
			const drk::Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			Engine::EngineState& engineState
		);
		static entt::entity
		copyMaterialEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity);

	};
}