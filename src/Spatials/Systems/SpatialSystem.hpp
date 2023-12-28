#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Models/Spatial.hpp"
#include "../Components/Spatial.hpp"
#include "../../Systems/System.hpp"
#include "../../Objects/Components/Relationship.hpp"

namespace drk::Spatials::Systems {
class SpatialSystem : public drk::Systems::System<Models::Spatial, Components::Spatial> {
	protected:
		const Devices::DeviceContext& deviceContext;

	public:
		SpatialSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		static void AddSpatialSystem(entt::registry& registry);
		static void RemoveSpatialSystem(entt::registry& registry);
		static void OnSpatialConstruct(entt::registry& registry, entt::entity spatialEntity);
		void Update(Models::Spatial& spatialModel, const Components::Spatial& spatial);
		void PropagateChanges();
		void MakeDirty(entt::entity entity);
		bool IsParent(entt::entity left, entt::entity right);
		std::string GetPath(entt::entity entity);
		static uint32_t GetDepth(const entt::registry& registry, entt::entity entity);
		//static bool compareRelationship(const entt::registry& registry, const entt::entity leftEntity, const entt::entity rightEntity);
	};
}