#pragma once

#include "../Components/LightPerspective.hpp"
#include "../Models/LightPerspective.hpp"
#include "../../Systems/StorageSystem.hpp"
#include "../../Common/KGuillotineAllocator.hpp"
#include "./ShadowMappingSystem.hpp"

namespace drk::Lights::Systems {
	class LightPerspectiveSystem : public drk::Systems::StorageSystem<
		Models::LightPerspective,
		Components::LightPerspective
	> {
	public:
		LightPerspectiveSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry,
			ShadowMappingSystem& shadowMappingSystem
		);
		void update(
			Models::LightPerspective& model,
			const Components::LightPerspective& lightPerspective
		);
		void processDirtyItems();
	protected:
		ShadowMappingSystem& shadowMappingSystem;
	};
}