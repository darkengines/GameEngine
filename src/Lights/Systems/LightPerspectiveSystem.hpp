#pragma once

#include "../Components/LightPerspective.hpp"
#include "../Models/LightPerspective.hpp"
#include "../../Systems/System.hpp"
#include "../../Common/KGuillotineAllocator.hpp"
#include "./ShadowMappingSystem.hpp"

namespace drk::Lights::Systems {
	class LightPerspectiveSystem : public drk::Systems::System<
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
		void Update(
			Models::LightPerspective& model,
			const Components::LightPerspective& lightPerspective
		);
		void ProcessDirtyItems();
	protected:
		ShadowMappingSystem& shadowMappingSystem;
	};
}