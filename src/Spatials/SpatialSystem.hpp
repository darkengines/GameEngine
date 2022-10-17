#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Graphics/EngineState.hpp"
#include "Models/Spatial.hpp"
#include "Spatial.hpp"

namespace drk::Spatials {
	class SpatialSystem {
	protected:
		Devices::DeviceContext *DeviceContext;
		Graphics::EngineState *EngineState;
		static void UpdateStoreItem(const Spatial& spatial, Models::Spatial &spatialModel);

	public:
		SpatialSystem(Devices::DeviceContext *pContext, Graphics::EngineState *pState);
		static void AddSpatialSystem(entt::registry &registry);
		static void RemoveSpatialSystem(entt::registry &registry);
		static void OnSpatialConstruct(entt::registry &registry, entt::entity spatialEntity);
		void UploadSpatials();
		void UpdateSpatials();
		void StoreSpatials();
		void PropagateChanges();
	};
}