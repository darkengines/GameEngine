#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Graphics/EngineState.hpp"

namespace drk::Relationships {
	class RelationshipSystem {
	protected:
		Devices::DeviceContext *DeviceContext;
		Graphics::EngineState *EngineState;

	public:
		RelationshipSystem(Devices::DeviceContext *pContext, Graphics::EngineState *pState);
		static void AddSpatialSystem(entt::registry &registry);
		static void RemoveSpatialSystem(entt::registry &registry);
		static void OnSpatialConstruct(entt::registry &registry, entt::entity spatialEntity);
		void SortRelationships();
	};
}