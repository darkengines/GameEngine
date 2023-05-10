#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "Models/Object.hpp"
#include "Object.hpp"
#include "../Meshes/Models/Mesh.hpp"
#include "../Spatials/Models/Spatial.hpp"
#include "../Systems/System.hpp"

namespace drk::Objects {
	class ObjectSystem : public Systems::System<Models::Object, Stores::StoreItem<Spatials::Models::Spatial>> {
	protected:
		const Devices::DeviceContext& DeviceContext;
		void Update(
			Models::Object& objectModel, const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
		);

	public:
		ObjectSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		static void AddObjectSystem(entt::registry& registry);
		static void RemoveObjectSystem(entt::registry& registry);
		static void OnObjectConstruct(entt::registry& registry, entt::entity objectEntity);
	};
}