#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "Models/Spatial.hpp"
#include "Spatial.hpp"

namespace drk::Spatials {
	class SpatialSystem {
	protected:
		const Devices::DeviceContext& DeviceContext;
		Engine::EngineState& EngineState;
		entt::registry& Registry;
		static void UpdateStoreItem(const Spatial& spatial, Models::Spatial &spatialModel);

	public:
		SpatialSystem(const Devices::DeviceContext& deviceContext, Engine::EngineState& engineState, entt::registry& registry);
		static void AddSpatialSystem(entt::registry &registry);
		static void RemoveSpatialSystem(entt::registry &registry);
		static void OnSpatialConstruct(entt::registry &registry, entt::entity spatialEntity);
		void UploadSpatials();
		void UpdateSpatials();
		void StoreSpatials();
		void PropagateChanges();
		bool IsParent(entt::entity left, entt::entity right);
		std::string GetPath(entt::entity entity);
		uint32_t GetDepth(entt::entity entity);
	};
}