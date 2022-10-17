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
		Devices::DeviceContext *DeviceContext;
		Graphics::EngineState *EngineState;
		void UpdateStoreItem(
			Models::Object &objectModel, const Stores::StoreItem<Spatials::Models::Spatial> &spatialStoreItem
		);

	public:
		ObjectSystem(Devices::DeviceContext *pContext, Graphics::EngineState *pState);
		static void AddObjectSystem(entt::registry &registry);
		static void RemoveObjectSystem(entt::registry &registry);
		static void OnObjectConstruct(entt::registry &registry, entt::entity objectEntity);
		void UpdateObjects();
		void StoreObjects();
	};
}