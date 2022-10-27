#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Graphics/EngineState.hpp"
#include "Models/Object.hpp"
#include "Object.hpp"
#include "../Meshes/Models/Mesh.hpp"
#include "../Spatials/Models/Spatial.hpp"

namespace drk::Objects {
	class ObjectSystem {
	protected:
		const Devices::DeviceContext& DeviceContext;
		Graphics::EngineState& EngineState;
		entt::registry& Registry;
		void UpdateStoreItem(
			Models::Object &objectModel, const Stores::StoreItem<Spatials::Models::Spatial> &spatialStoreItem
		);

	public:
		ObjectSystem(const Devices::DeviceContext& deviceContext, Graphics::EngineState& engineState, entt::registry& registry);
		static void AddObjectSystem(entt::registry &registry);
		static void RemoveObjectSystem(entt::registry &registry);
		static void OnObjectConstruct(entt::registry &registry, entt::entity objectEntity);
		void UpdateObjects();
		void StoreObjects();
	};
}