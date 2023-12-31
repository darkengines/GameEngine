#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Models/Object.hpp"
#include "../Components/Object.hpp"
#include "../../Meshes/Models/Mesh.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "../../Systems/System.hpp"
#include "../../Loaders/LoadResult.hpp"

namespace drk::Objects::Systems {
class ObjectSystem : public drk::Systems::System<Models::Object, Stores::StoreItem<Spatials::Models::Spatial>> {
	protected:
		const Devices::DeviceContext& DeviceContext;
		void update(
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
		static entt::entity copyObjectEntity(
			const entt::registry& source,
			entt::registry& destination,
			entt::entity sourceEntity,
			entt::entity parent = entt::null,
			entt::entity previousSibling = entt::null
		);
	};
}