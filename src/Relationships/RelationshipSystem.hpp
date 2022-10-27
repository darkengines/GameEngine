#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Graphics/EngineState.hpp"

namespace drk::Relationships {
	class RelationshipSystem {
	protected:
		const Devices::DeviceContext& DeviceContext;
		Graphics::EngineState& EngineState;
		entt::registry& Registry;

	public:
		RelationshipSystem(const Devices::DeviceContext& deviceContext, Graphics::EngineState& engineState, entt::registry& registry);
		static void AddSpatialSystem(entt::registry &registry);
		static void RemoveSpatialSystem(entt::registry &registry);
		static void OnSpatialConstruct(entt::registry &registry, entt::entity spatialEntity);
		void SortRelationships();
	};
}