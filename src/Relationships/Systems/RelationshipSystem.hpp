#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"

namespace drk::Relationships::Systems {
	class RelationshipSystem {
	protected:
		const Devices::DeviceContext& DeviceContext;
		Engine::EngineState& EngineState;
		entt::registry& registry;

	public:
		RelationshipSystem(const Devices::DeviceContext& deviceContext, Engine::EngineState& engineState, entt::registry& registry);
		static void AddSpatialSystem(entt::registry &registry);
		static void RemoveSpatialSystem(entt::registry &registry);
		static void OnSpatialConstruct(entt::registry &registry, entt::entity spatialEntity);
		void SortRelationships();
	};
}